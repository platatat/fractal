#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "complex.h"
#include "tile.h"
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <thread>
#include <unordered_map>
#include <vector>


class TileManager {
private:
    unsigned int _cache_size;

    std::deque<TileHeader> _request_queue;

    std::mutex _mutex;

    std::condition_variable _request_queue_nonempty;

    std::thread _worker_thread;

    static void tileLoadingTask(TileManager* tile_manager);

    bool requestQueueContains(TileHeader header);

    void cacheInsert(Tile* tile);

    bool cacheContains(TileHeader header);

    void cacheEvictOldest();

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

    std::unordered_map<TileHeader, CachedTile, TileHeaderHasher> _cache;

    static Tile* generateTile(TileHeader header);

public:
    TileManager(unsigned int cache_size = 16);

    Tile* requestTile(TileHeader header);

    struct ViewportInfo {
        int tiles_width, tiles_height;
    };

    ViewportInfo loadViewport(complex origin, complex size, int z, std::vector<Tile*>& tiles);
};


#endif
