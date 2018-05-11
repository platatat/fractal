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
#include <unordered_set>
#include <vector>


class TileManager {
private:
    int _cache_size;
    int _request_depth;

    TileClient _tile_client;

    TileRequestHeap _request_heap;
    std::unordered_set<std::shared_ptr<TileHeader>, TileHeader::Hasher, TileHeader::Comparator> _outstanding_requests;

    std::mutex _mutex;
    std::condition_variable _requests_nonempty;
    std::condition_variable _requests_available;
    std::thread _tile_requesting_thread;
    std::thread _tile_receiving_thread;

    unsigned char* _placeholder_data;

    static void tileRequestingTask(TileManager* tile_manager);

    static void tileReceivingTask(TileManager* tile_manager);

    static void loadPlaceholder(unsigned char* data_buffer);

    bool isTileRequested(std::shared_ptr<TileHeader> header);

    void cacheInsert(std::shared_ptr<Tile> tile);

    bool cacheContains(std::shared_ptr<TileHeader> header);

    void cacheEvictOldest();

    struct CachedTile {
        std::shared_ptr<Tile> tile;
        std::chrono::time_point<std::chrono::system_clock> last_hit;
    };

    std::unordered_map<std::shared_ptr<TileHeader>, CachedTile, TileHeader::Hasher, TileHeader::Comparator> _cache;

public:
    TileManager(int cache_size, int request_depth = 4);

    ~TileManager();

    std::shared_ptr<Tile> requestTile(std::shared_ptr<TileHeader> header);

    struct ViewportInfo {
        int tiles_width, tiles_height;
        double fractional_x, fractional_y;
    };

    ViewportInfo loadViewport(complex origin, complex size, int z, std::vector<std::shared_ptr<Tile>>& tiles);
};


#endif
