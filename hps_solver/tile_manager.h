#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "complex.h"
#include "tile.h"
#include "tile_request_heap.h"
#include "tile_client.h"
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

    TileClient _tile_client;

    TileRequestHeap _request_heap;

    std::shared_ptr<TileHeader> _current_request;

    std::mutex _mutex;

    std::condition_variable _requests_nonempty;

    std::thread _worker_thread;

    unsigned char* _placeholder_data;

    static void tileLoadingTask(TileManager* tile_manager);

    static void loadPlaceholder(unsigned char* data_buffer);

    bool requestQueueContains(std::shared_ptr<TileHeader> header);

    void cacheInsert(std::shared_ptr<Tile> tile);

    bool cacheContains(std::shared_ptr<TileHeader> header);

    void cacheEvictOldest();

    struct CachedTile {
        std::shared_ptr<Tile> tile;
        std::chrono::time_point<std::chrono::system_clock> last_hit;
    };

    struct TileHeaderHasher {
        std::size_t operator()(const std::shared_ptr<TileHeader>& header) const {
            size_t h = 0;
            // TODO: smarter way of hashing big ints would be good.
            h = header->x.get_si() + (h << 6) + (h << 16) - h;
            h = header->y.get_si() + (h << 6) + (h << 16) - h;
            h = header->z + (h << 6) + (h << 16) - h;
            return h;
        }
    };

    struct TileHeaderComparator {
        bool operator()(const std::shared_ptr<TileHeader>& a, const std::shared_ptr<TileHeader>& b) const {
            return (a->x == b->x) && (a->y == b->y) && (a->z == b->z);
        }
    };

    std::unordered_map<std::shared_ptr<TileHeader>, CachedTile, TileHeaderHasher, TileHeaderComparator> _cache;

    static std::shared_ptr<Tile> generateTile(std::shared_ptr<TileHeader> header);

public:
    TileManager(unsigned int cache_size = 16);

    ~TileManager();

    std::shared_ptr<Tile> requestTile(std::shared_ptr<TileHeader> header);

    struct ViewportInfo {
        int tiles_width, tiles_height;
        double fractional_x, fractional_y;
    };

    ViewportInfo loadViewport(complex origin, complex size, int z, std::vector<std::shared_ptr<Tile>>& tiles);
};


#endif
