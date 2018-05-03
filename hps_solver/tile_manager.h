#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "complex.h"
#include "tile.h"
#include "tile_request_heap.h"
#include <algorithm>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <vector>


class TileManager {
private:
    unsigned int _cache_size;

    TileRequestHeap _request_heap;

    TileHeader _current_request;

    std::mutex _mutex;

    std::condition_variable _requests_nonempty;

    std::thread _worker_thread;

    unsigned char* _placeholder_data;

    static void tileLoadingTask(TileManager* tile_manager);

    static void loadPlaceholder(unsigned char* data_buffer);

    bool requestQueueContains(TileHeader header);

    void cacheInsert(std::shared_ptr<Tile> tile);

    bool cacheContains(TileHeader header);

    void cacheEvictOldest();

    struct CachedTile {
        std::shared_ptr<Tile> tile;
        std::chrono::time_point<std::chrono::system_clock> last_hit;
    };

    struct TileHeaderHasher {
        std::size_t operator()(const TileHeader& header) const {
            size_t h = 0;
            // TODO: smarter way of hashing big ints would be good.
            h = header.x.get_si() + (h << 6) + (h << 16) - h;
            h = header.y.get_si() + (h << 6) + (h << 16) - h;
            h = header.z + (h << 6) + (h << 16) - h;
            return h;
        }
    };

    std::unordered_map<TileHeader, CachedTile, TileHeaderHasher> _cache;

    static std::shared_ptr<Tile> generateTile(TileHeader header);

public:
    TileManager(unsigned int cache_size = 16);

    ~TileManager();

    std::shared_ptr<Tile> requestTile(TileHeader header);

    struct ViewportInfo {
        int tiles_width, tiles_height;
        double fractional_x, fractional_y;
    };

    ViewportInfo loadViewport(complex origin, complex size, int z, std::vector<std::shared_ptr<Tile>>& tiles);
};


#endif
