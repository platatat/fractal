#include "tile_manager.h"
#include "tile_solver.h"
#include <algorithm>
#include <math.h>
#include <iostream>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(unsigned int cache_size) {
    _cache_size = cache_size;
    _worker_thread = std::thread(tileLoadingTask, this);
}


void TileManager::tileLoadingTask(TileManager* tile_manager) {
    std::unique_lock<std::mutex> lock(tile_manager->_mutex);

    while (true) {
        // Wait for queue to be non-empty.
        tile_manager->_request_queue_nonempty.wait(lock);

        while (tile_manager->_request_queue.size() > 0) {
            TileHeader header = tile_manager->_request_queue.front();

            // Release lock during tile computation.
            lock.unlock();
            Tile* tile = generateTile(header);

            // Acquire lock to access the cache.
            lock.lock();
            tile_manager->cacheInsert(tile);
            tile_manager->_request_queue.pop_front();
        }
    }
}


bool TileManager::requestQueueContains(TileHeader header) {
    auto find_result = std::find(_request_queue.begin(), _request_queue.end(), header);
    return find_result != _request_queue.end();
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
            _request_queue.push_back(header);
            _request_queue_nonempty.notify_one();
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


TileManager::ViewportInfo TileManager::loadViewport(complex origin, complex size, int z, 
                                                    std::vector<Tile*>& tiles) {
    std::unique_lock<std::mutex> lock(_mutex);

    double tile_length = pow(2, -z);
    int left = (int) origin.real.toDouble() / tile_length;
    int right = (int) (origin.real + size.real).toDouble() / tile_length;
    int bottom = (int) origin.imag.toDouble() / tile_length;
    int top = (int) (origin.imag + size.imag).toDouble() / tile_length;

    tiles.clear();
    for (int y = bottom; y <= top; y++) {
        for (int x = left; x <= right; x++) {
            Tile* tile = requestTile({x, y, z});
            tiles.push_back(tile);
        }
    }

    ViewportInfo out_info = {0, };

    out_info.tiles_width = right - left + 1;
    out_info.tiles_height = top - bottom + 1;

    return out_info;
}
