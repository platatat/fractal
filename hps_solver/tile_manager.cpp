#include "tile_manager.h"
#include "tile_solver.h"
#include <math.h>
#include <iostream>
#include <png.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(unsigned int cache_size) : _request_heap(64), _cache_size(cache_size) {
    _placeholder_data = new unsigned char [Constants::TILE_WIDTH * Constants::TILE_HEIGHT];
    loadPlaceholder(_placeholder_data);

    _worker_thread = std::thread(tileLoadingTask, this);
}


TileManager::~TileManager() {
    delete[] _placeholder_data;
}


void TileManager::tileLoadingTask(TileManager* tile_manager) {
    std::unique_lock<std::mutex> lock(tile_manager->_mutex);

    while (true) {
        // Wait for a tile request.
        tile_manager->_requests_nonempty.wait(lock);

        while (tile_manager->_request_heap.size() > 0) {
            tile_manager->_current_request = tile_manager->_request_heap.pop();

            // Release lock during tile computation.
            lock.unlock();
            std::shared_ptr<Tile> tile = generateTile(tile_manager->_current_request);

            // Acquire lock to access the cache.
            lock.lock();
            tile_manager->cacheInsert(tile);
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


bool TileManager::requestQueueContains(TileHeader header) {
    return _request_heap.contains(header) || header == _current_request;
}


void TileManager::cacheInsert(std::shared_ptr<Tile> tile) {
    while (_cache.size() >= _cache_size - 1) {
        cacheEvictOldest();
    }

    _cache[tile->getHeader()] = {tile, system_clock::now()};
}


bool TileManager::cacheContains(TileHeader header) {
    return _cache.find(header) != _cache.end();
}


void TileManager::cacheEvictOldest() {
    std::cout << "Evicting\n";

    time_point<system_clock> oldest_time = system_clock::now();
    TileHeader oldest_header;
    std::shared_ptr<Tile> oldest_tile;

    for (std::pair<TileHeader, CachedTile> element : _cache) {
        if (element.second.last_hit < oldest_time) {
            oldest_tile = element.second.tile;
            oldest_time = element.second.last_hit;
            oldest_header = element.first;
        }
    }

    delete[] oldest_tile->getData();
    _cache.erase(oldest_header);
}


std::shared_ptr<Tile> TileManager::requestTile(TileHeader header) {
    if (cacheContains(header)) {
        auto cache_result = _cache.find(header);
        cache_result->second.last_hit = system_clock::now();
        return cache_result->second.tile;
    }
    else {
        if (!requestQueueContains(header)) {
            _request_heap.push(header);
            _requests_nonempty.notify_one();
        }

        std::shared_ptr<Tile> placeholder_tile = std::make_shared<Tile>(header, _placeholder_data, true);

        return placeholder_tile;
    }
}


std::shared_ptr<Tile> TileManager::generateTile(TileHeader header) {
    std::cout << "Generating (" << header.x.get_si() << ", " << header.y.get_si() << ", " << header.z << ")\n";

    unsigned char* tile_data = new unsigned char [Constants::TILE_PIXELS];
    std::shared_ptr<Tile> tile = std::make_shared<Tile>(header, tile_data);
    TileSolver::solveTile(tile, Constants::ITERATIONS);

    return tile;
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
            std::shared_ptr<Tile> tile = requestTile({x, y, viewport_z});
            tiles.push_back(tile);
            // break;
        }
        // break;
    }

    // // Pre-fetching for tiles close to the current viewport.
    // for (int y = bottom; y <= top; y++) {
    //     for (int x = left; x <= right; x++) {
    //         TileHeader header = {x, y, z};
    //         if (!cacheContains(header) && !requestQueueContains(header)) {
    //             _request_heap.push(header);
    //             _requests_nonempty.notify_one();
    //         }
    //     }
    // }

    // Re-prioritize the tile request heap based on the current viewport.
    _request_heap.rebuild([left, right, top, bottom, viewport_z](TileHeader& header) {
        double x_dist = std::abs(header.x.get_si() - (left.get_si() + right.get_si()) * 0.5);
        double y_dist = std::abs(header.y.get_si() - (bottom.get_si() + top.get_si()) * 0.5);
        double z_dist = std::abs(header.z - viewport_z);
        return x_dist + y_dist + 4 * z_dist;
    });

    ViewportInfo out_info = {0, };

    out_info.tiles_width = right.get_si() - left.get_si() + 1;
    out_info.tiles_height = top.get_si() - bottom.get_si() + 1;

    out_info.fractional_x = left_float.get_d() - left.get_si();
    out_info.fractional_y = bottom_float.get_d() - bottom.get_si();

    return out_info;
}
