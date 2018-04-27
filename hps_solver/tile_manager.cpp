#include "tile_manager.h"
#include "tile_solver.h"
#include <math.h>
#include <iostream>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(unsigned int cache_size) : _request_heap(64) {
    _cache_size = cache_size;
    _worker_thread = std::thread(tileLoadingTask, this);
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
            Tile* tile = generateTile(tile_manager->_current_request);

            // Acquire lock to access the cache.
            lock.lock();
            tile_manager->cacheInsert(tile);
        }
    }
}


bool TileManager::requestQueueContains(TileHeader header) {
    return _request_heap.contains(header) || header == _current_request;
}


void TileManager::cacheInsert(Tile* tile) {
    while (_cache.size() >= _cache_size - 1) {
        cacheEvictOldest();
    }

    _cache[tile->getHeader()] = {tile, system_clock::now()};
}


bool TileManager::cacheContains(TileHeader header) {
    return _cache.find(header) != _cache.end();
}


void TileManager::cacheEvictOldest() {
    time_point<system_clock> oldest_time = system_clock::now();
    TileHeader oldest_header;

    for (std::pair<TileHeader, CachedTile> element : _cache) {
        if (element.second.last_hit < oldest_time) {
            oldest_time = element.second.last_hit;
            oldest_header = element.first;
        }
    }

    _cache.erase(oldest_header);
}


Tile* TileManager::requestTile(TileHeader header) {
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
        return nullptr;
    }
}


Tile* TileManager::generateTile(TileHeader header) {
    std::cout << "Generating (" << header.x << ", " << header.y << ", " << header.z << ")\n";

    Tile* tile = new Tile(header);
    TileSolver::solveTile(tile, Constants::ITERATIONS);

    return tile;
}


TileManager::ViewportInfo TileManager::loadViewport(complex origin, complex size, int viewport_z, 
                                                    std::vector<Tile*>& tiles) {
    std::unique_lock<std::mutex> lock(_mutex);

    double tile_length = pow(2, -viewport_z);

    double left_edge   = origin.real.toDouble() / tile_length;
    double bottom_edge = origin.imag.toDouble() / tile_length;

    int left = (int) std::floor(left_edge);
    int right = (int) std::floor((origin.real + size.real).toDouble() / tile_length);

    int bottom = (int) std::floor(bottom_edge);
    int top = (int) std::floor((origin.imag + size.imag).toDouble() / tile_length);

    tiles.clear();
    _request_heap.clear();

    // Request tiles for current viewport.
    for (int y = bottom; y <= top; y++) {
        for (int x = left; x <= right; x++) {
            Tile* tile = requestTile({x, y, viewport_z});
            tiles.push_back(tile);
        }
    }

    // Pre-fetching for tiles close to the current viewport.
    for (int z = viewport_z - 1; z <= viewport_z + 1; z++) {
        for (int y = bottom - 1; y <= top + 1; y++) {
            for (int x = left - 1; x <= right + 1; x++) {
                TileHeader header = {x, y, z};
                if (!cacheContains(header) && !requestQueueContains(header)) {
                    _request_heap.push(header);
                    _requests_nonempty.notify_one();
                }
            }
        }
    }

    // Re-prioritize the tile request heap based on the current viewport.
    _request_heap.rebuild([left, right, top, bottom, viewport_z](TileHeader& header) {
        double x_dist = std::abs(header.x - 0.5 * (left + right));
        double y_dist = std::abs(header.y - 0.5 * (bottom + top));
        double z_dist = std::abs(header.z - viewport_z);
        return x_dist + y_dist + 4 * z_dist;
    });

    ViewportInfo out_info = {0, };

    out_info.tiles_width = right - left + 1;
    out_info.tiles_height = top - bottom + 1;

    out_info.fractional_x = left_edge - left;
    out_info.fractional_y = bottom_edge - bottom;

    return out_info;
}
