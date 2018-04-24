#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "complex.h"
#include "tile.h"
#include <vector>
#include <unordered_map>
#include <chrono>


class TileManager {
private:
    Tile* generateTile(TileHeader header);

    void evictOldest();

    struct CachedTile {
        Tile* tile;
        std::chrono::time_point<std::chrono::system_clock> last_hit;
    };

    struct TileHeaderHasher {
        std::size_t operator()(const TileHeader& header) const {
            size_t h = 0;
            h = header.x + (h << 6) + (h << 16) - h;
            h = header.y + (h << 6) + (h << 16) - h;
            h = header.z + (h << 6) + (h << 16) - h;
            return h;
        }
    };

    unsigned int _cache_size;

    std::unordered_map<TileHeader, CachedTile, TileHeaderHasher> _cache;

public:
    TileManager();

    TileManager(unsigned int cache_size);

    Tile* requestTile(TileHeader header);

    void loadViewport(complex origin, complex size, int z, std::vector<Tile*>& tiles);
};


#endif
