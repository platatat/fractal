#include "tile_manager.h"

#include "constants.h"

#include <math.h>
#include <iostream>
#include <png.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager(std::vector<std::tuple<std::string, int>> ip_addrs, int cache_size, int request_depth) :
        cache_size(cache_size),
        request_depth(request_depth),
        iterations(Constants::ITERATIONS),
        _request_heap(64) {

    for (unsigned int i = 0; i < ip_addrs.size(); i++) {
        std::string ip_addr = std::get<0>(ip_addrs[i]);
        int port = std::get<1>(ip_addrs[i]);
        clients.emplace_back(ip_addr, port);
        clients[i].init();
        requesting_threads.emplace_back(tileRequestingTask, this, i);
        receiving_threads.emplace_back(tileReceivingTask, this, i);
    }
}


void TileManager::tileRequestingTask(TileManager* manager, unsigned int i) {
    while(true) {
        std::shared_ptr<TileHeader> header;

        {
            std::unique_lock<std::mutex> lock(manager->_mutex);

            // Wait for a request from the viewport.
            while (manager->_request_heap.size() == 0) {
                manager->_requests_nonempty.wait(lock);
            }

            if (manager->_request_heap.size() == 0) std::cout << "Empty request heap" << std::endl;

            std::cout << "requesting from " << manager->clients[i].get_ip_addr();
            std::cout << ":" << manager->clients[i].get_port() << std::endl;

            header = manager->_request_heap.front();
            manager->_request_heap.pop();
            manager->_outstanding_requests.insert(header);
        }

        // std::cout << "requesting " << header->get_str() << std::endl;

        manager->clients[i].requestTile(header);
    }
}


void TileManager::tileReceivingTask(TileManager* manager, unsigned int i) {
    // unsigned int outstanding_request_count = 0;

    while (true) {
        // Receive a tile.
        std::unique_ptr<Tile> unique_tile = manager->clients[i].receiveTile();

        std::cout << "received tile from " << manager->clients[i].get_ip_addr();
        std::cout << ":" << manager->clients[i].get_port() << std::endl;

        std::shared_ptr<Tile> tile = std::move(unique_tile); // TODO: this unique ptr shit is dumb.
        std::shared_ptr<TileHeader> header = tile->getHeader();

        {
            // Put the tile in the cache and erase it from outstanding requests.
            std::unique_lock<std::mutex> lock(manager->_mutex);
            // std::cout << " (" << outstanding_request_count << ")" << std::endl;
            manager->cacheInsert(tile);
            manager->_outstanding_requests.erase(header);
            manager->_requests_available.notify_all();
        }
    }
}


bool TileManager::isTileRequested(std::shared_ptr<TileHeader> header) {
    bool is_requested = _request_heap.contains(header);
    bool is_outstanding = _outstanding_requests.find(header) != _outstanding_requests.end();
    return is_requested || is_outstanding;
}


void TileManager::cacheInsert(std::shared_ptr<Tile> tile) {
    while (_cache.size() >= cache_size - 1) {
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

    // std::cout << "evicting data " << oldest_header->get_str() << std::endl;
    _cache.erase(oldest_header);
}


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

        // Get a lower resolution tile if possible, or return a placeholder tile with no data.
        if (depth > 0) {
            auto parent_header = std::make_shared<TileHeader>(header->x >> 1, header->y >> 1, header->z - 1, header->i);
            return requestTile(parent_header, depth - 1);
        }
        else {
            return std::make_shared<Tile>(header);
        }
    }
}


std::set<std::shared_ptr<Tile>> TileManager::loadViewport(Viewport viewport) {
    std::unique_lock<std::mutex> lock(_mutex);

    std::set<std::shared_ptr<Tile>> tiles;

    // Request tiles for current viewport.
    for (int tile_y = 0; tile_y < viewport.height; tile_y++) {
        for (int tile_x = 0; tile_x < viewport.width; tile_x++) {
            mpz_class x = viewport.origin_x + tile_x;
            mpz_class y = viewport.origin_y + tile_y;

            std::shared_ptr<TileHeader> header = std::make_shared<TileHeader>(x, y, viewport.zoom, iterations);
            std::shared_ptr<Tile> tile = requestTile(header, 0);

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


void TileManager::clearRequests() {
    std::lock_guard<std::mutex> lock(_mutex);
    _request_heap.clear();
}
