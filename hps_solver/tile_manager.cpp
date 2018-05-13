#include "tile_manager.h"
#include "tile_solver.h"
#include <math.h>
#include <iostream>
#include <png.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(int cache_size, int request_depth) : 
        _request_heap(64), 
        _cache_size(cache_size), 
        _tile_client(Constants::PORT),
        _request_depth(request_depth) {
    _tile_client.init();

    _placeholder_data = new unsigned char [Constants::TILE_WIDTH * Constants::TILE_HEIGHT];
    loadPlaceholder(_placeholder_data);

    _tile_requesting_thread = std::thread(tileRequestingTask, this);
    _tile_receiving_thread = std::thread(tileReceivingTask, this);
}


TileManager::~TileManager() {
    delete[] _placeholder_data;
}


void TileManager::tileRequestingTask(TileManager* tile_manager) {
    while(true) {
        std::shared_ptr<TileHeader> header;
        bool tile_requested;

        {
            std::unique_lock<std::mutex> lock(tile_manager->_mutex);

            // Wait for a request from the viewport.
            while (tile_manager->_request_heap.size() == 0) {
                tile_manager->_requests_nonempty.wait(lock);
            }

            // Wait for space on the server request queue.
            while (tile_manager->_outstanding_requests.size() >= tile_manager->_request_depth) {
                tile_manager->_requests_available.wait(lock);
            }
            
            // Get the highest priority tile request.
            tile_requested = tile_manager->_request_heap.size() > 0;
            if (tile_requested) {
                header = tile_manager->_request_heap.front();
                tile_manager->_request_heap.pop();
                tile_manager->_outstanding_requests.insert(header);
            }
            
        }
        
        // Send the tile request to the server.
        if (tile_requested) {
            tile_manager->_tile_client.requestTile(header);
        }
    }
}


void TileManager::tileReceivingTask(TileManager* tile_manager) {
    while (true) {
        std::unique_ptr<Tile> unique_tile = tile_manager->_tile_client.receiveTile();
        std::shared_ptr<Tile> tile = std::move(unique_tile);
        std::shared_ptr<TileHeader> header = tile->getHeader();

        {
            std::unique_lock<std::mutex> lock(tile_manager->_mutex);
            std::cout << "adding tile " << header->get_str() << std::endl;
            tile_manager->cacheInsert(tile);
            tile_manager->_outstanding_requests.erase(header);
            tile_manager->_requests_available.notify_one();
        }
    }
}


void TileManager::loadPlaceholder(unsigned char* data_buffer) {
    SDL_Surface* surface = IMG_Load("../data/land.jpg");

    if (surface == nullptr) {
        std::cout << "IMG_Load: " << IMG_GetError() << "\n";
        return;
    }

    int sx = 2;
    int sy = 2;
    int ox = 140;
    int oy = 20;
    
    for (int y = 0; y < Constants::TILE_HEIGHT; y++) {
        for (int x = 0; x < Constants::TILE_WIDTH; x++) {
            int bpp = surface->format->BytesPerPixel;
            Uint8 *p = (Uint8*) surface->pixels + (sy * y + oy) * surface->pitch + (sx * x + ox) * bpp;
            data_buffer[y * Constants::TILE_WIDTH + x] = *p;
        }
    }
}


bool TileManager::isTileRequested(std::shared_ptr<TileHeader> header) {
    bool is_requested = _request_heap.contains(header);
    bool is_outstanding = _outstanding_requests.find(header) != _outstanding_requests.end();
    return is_requested || is_outstanding;
}


void TileManager::cacheInsert(std::shared_ptr<Tile> tile) {
    while (_cache.size() >= _cache_size - 1) {
        cacheEvictOldest();
    }

    _cache[tile->getHeader()] = {tile, system_clock::now()};
}


bool TileManager::cacheContains(std::shared_ptr<TileHeader> header) {
    return _cache.find(header) != _cache.end();
}


void TileManager::cacheEvictOldest() {
    time_point<system_clock> oldest_time = system_clock::now();
    std::shared_ptr<TileHeader> oldest_header;
    std::shared_ptr<Tile> oldest_tile;

    for (std::pair<std::shared_ptr<TileHeader>, CachedTile> element : _cache) {
        if (element.second.last_hit < oldest_time) {
            oldest_tile = element.second.tile;
            oldest_time = element.second.last_hit;
            oldest_header = element.first;
        }
    }

    std::cout << "evicting " << oldest_header->get_str() << std::endl;
    _cache.erase(oldest_header);
}


// TODO: find a better way to do this, this is horrible.
std::shared_ptr<Tile> TileManager::requestTile(std::shared_ptr<TileHeader> header, int depth) {
    if (cacheContains(header)) {
        auto cache_result = _cache.find(header);
        cache_result->second.last_hit = system_clock::now();
        return cache_result->second.tile;
    }
    else {
        // Request tile if it hasn't been requested yet.
        if (!isTileRequested(header)) {
            _request_heap.push(header);
            _requests_nonempty.notify_one();
        }

        if (depth > 0) {
            // Get a lower resolution tile if possible.
            auto parent_header = std::make_shared<TileHeader>(header->x >> 1, header->y >> 1, header->z - 1);
            // auto lowres_header = std::make_shared<TileHeader>(original_header->x, original_header->y, header->z - 1);
            return requestTile(parent_header, depth - 1);
            
            // if (parent_tile->hasData()) {
            //     return parent_tile;
            // }
            // else {
            //     return std::make_shared<Tile>(lowres_header);
            // }
            
            // if (cacheContains(parent_header)) {
            //     auto cache_result = _cache.find(parent_header);
            //     cache_result->second.last_hit = system_clock::now();
            //     std::vector<uint8_t> tile_data = cache_result->second.tile->getData();
            //     return std::make_shared<Tile>(lowres_header, tile_data);
            // }
        }
        
        else {
            // Otherwise return a placeholder tile with no data.
            return std::make_shared<Tile>(header);
        }
    }
}


std::set<std::shared_ptr<Tile>> TileManager::loadViewport(Viewport viewport) {
    std::unique_lock<std::mutex> lock(_mutex);

    std::set<std::shared_ptr<Tile>> tiles;

    tiles.clear();
    _request_heap.clear();

    // Request tiles for current viewport.
    for (int tile_y = 0; tile_y < viewport.height; tile_y++) {
        for (int tile_x = 0; tile_x < viewport.width; tile_x++) {
            mpz_class x = viewport.origin_x + tile_x;
            mpz_class y = viewport.origin_y + tile_y;

            std::shared_ptr<TileHeader> header = std::make_shared<TileHeader>(x, y, viewport.zoom);
            std::shared_ptr<Tile> r_tile = requestTile(header, 0);
            std::shared_ptr<TileHeader> r_header = r_tile->getHeader();
            auto tile_header = std::make_shared<TileHeader>(header->x, header->y, r_header->z);

            std::shared_ptr<Tile> tile;

            if (r_tile->hasData()) {
                tile = std::make_shared<Tile>(tile_header, r_tile->getData());
            }
            else {
                tile = std::make_shared<Tile>(tile_header);
            }

            tiles.insert(tile);
        }
    }

    // Pre-fetching for tiles close to the current viewport.
    // for (int z = viewport_z; z <= viewport_z + 1; z++) {
    //     int zoom_scale = 1 << (z - viewport_z);
    //     mpz_class z_left    = left * zoom_scale;
    //     mpz_class z_bottom  = bottom * zoom_scale;
    //     mpz_class z_right   = right * zoom_scale;
    //     mpz_class z_top     = top * zoom_scale;

    //     for (mpz_class y = z_bottom; y <= z_top; y++) {
    //         for (mpz_class x = z_left; x <= z_right; x++) {
    //             TileHeader header = {x, y, z};
    //             if (!cacheContains(header) && !requestQueueContains(header)) {
    //                 _request_heap.push(header);
    //                 _requests_nonempty.notify_one();
    //             }
    //         }
    //     }
    // }

    // Re-prioritize the tile request heap based on the current viewport
    _request_heap.rebuild([viewport](std::shared_ptr<TileHeader> header) {
        int z_dist = header->z - viewport.zoom;

        // TODO: use real viewport center, not tile center.
        mpz_class center_x = viewport.origin_x + (viewport.width / 2);
        mpz_class center_y = viewport.origin_y + (viewport.height / 2);

        mpz_class header_x_eff = header->x / (1 << z_dist);
        mpz_class header_y_eff = header->y / (1 << z_dist);

        mpz_class x_dist = header_x_eff - center_x;
        mpz_class y_dist = header_y_eff - center_y;

        double loss_x = std::abs(x_dist.get_si());
        double loss_y = std::abs(y_dist.get_si());
        double loss_z = 4 * z_dist;

        return std::max(loss_x, loss_y) + loss_z;
    });

    return tiles;
}
