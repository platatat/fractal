#include "tile_manager.h"
#include "tile_solver.h"
#include <math.h>
#include <iostream>

using std::chrono::system_clock;
using std::chrono::time_point;


TileManager::TileManager() {
    _cache_size = 16;
}


TileManager::TileManager(unsigned int cache_size) {
    _cache_size = cache_size;
}


void TileManager::evictOldest() {
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
    // std::cout << "Requesting (" << header.x << ", " << header.y << ", " << header.z << ")\n";

    // If cache hit, update timestamps and return tile.
    auto cache_lookup = _cache.find(header);
    if (cache_lookup != _cache.end()) {
        cache_lookup->second.last_hit = system_clock::now();
        return cache_lookup->second.tile;
    }

    // If cache miss, make room for a new tile.
    while (_cache.size() >= _cache_size - 1) {
        evictOldest();
    }

    // Generate a new tile and add it to the cache.
    Tile* tile = generateTile(header);
    _cache[header] = {tile, system_clock::now()};
    return tile;
}


Tile* TileManager::generateTile(TileHeader header) {
    std::cout << "Generating (" << header.x << ", " << header.y << ", " << header.z << ")\n";

    Tile* tile = new Tile(header);
    TileSolver::solveTile(tile, Constants::ITERATIONS);

    return tile;
}


TileManager::ViewportInfo TileManager::loadViewport(complex origin, complex size, int z, std::vector<Tile*>& tiles) {
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
