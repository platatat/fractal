#include "tile_manager.h"
#include <math.h>
#include <iostream>
#include <png.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(std::string ip_addr, int cache_size, int request_depth) : 
        _request_heap(64), 
        _cache_size(cache_size), 
        _tile_client(ip_addr, Constants::PORT),
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


std::shared_ptr<Tile> TileManager::requestTile(std::shared_ptr<TileHeader> header) {
    if (cacheContains(header)) {
        auto cache_result = _cache.find(header);
        cache_result->second.last_hit = system_clock::now();
        return cache_result->second.tile;
    }
    else {
        if (!isTileRequested(header)) {
            _request_heap.push(header);
            _requests_nonempty.notify_one();
        }

        std::shared_ptr<Tile> placeholder_tile = std::make_shared<Tile>(header);

        return placeholder_tile;
    }
}


TileManager::ViewportInfo TileManager::loadViewport(complex origin, complex size, int viewport_z, 
                                                    std::vector<std::shared_ptr<Tile>>& tiles) {
    std::unique_lock<std::mutex> lock(_mutex);

    double tile_length = pow(2, -viewport_z);

    mpf_class left_float    = origin.real / tile_length;
    mpf_class bottom_float  = origin.imag / tile_length;
    mpf_class right_float   = (origin.real + size.real) / tile_length;
    mpf_class top_float     = (origin.imag + size.imag) / tile_length;

    mpz_class left(floor(left_float));
    mpz_class right(floor(right_float));
    mpz_class bottom(floor(bottom_float));
    mpz_class top(floor(top_float));

    tiles.clear();
    _request_heap.clear();

    // Request tiles for current viewport.
    for (mpz_class y = bottom; y <= top; y++) {
        for (mpz_class x = left; x <= right; x++) {
            std::shared_ptr<TileHeader> header = std::make_shared<TileHeader>(x, y, viewport_z);
            std::shared_ptr<Tile> tile = requestTile(header);
            tiles.push_back(tile);
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

    // Re-prioritize the tile request heap based on the current viewport.
    _request_heap.rebuild([left, right, top, bottom, viewport_z](std::shared_ptr<TileHeader> header) {
        int z_dist = header->z - viewport_z;

        mpz_class center_x = (left.get_si() + right.get_si()) / 2;
        mpz_class center_y = (bottom.get_si() + top.get_si()) / 2;

        mpz_class header_x_eff = header->x / (1 << z_dist);
        mpz_class header_y_eff = header->y / (1 << z_dist);

        mpz_class x_dist = header_x_eff - center_x;
        mpz_class y_dist = header_y_eff - center_y;

        double loss_x = std::abs(x_dist.get_si());
        double loss_y = std::abs(y_dist.get_si());
        double loss_z = 4 * z_dist;

        return std::max(loss_x, loss_y) + loss_z;
    });

    ViewportInfo out_info = {0, };

    out_info.tiles_width = right.get_si() - left.get_si() + 1;
    out_info.tiles_height = top.get_si() - bottom.get_si() + 1;

    out_info.fractional_x = left_float.get_d() - left.get_si();
    out_info.fractional_y = bottom_float.get_d() - bottom.get_si();

    return out_info;
}
