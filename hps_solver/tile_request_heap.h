#ifndef __TILE_REQUEST_HEAP_H__
#define __TILE_REQUEST_HEAP_H__

#include "tile_header.h"
#include <functional>
#include <vector>


class TileRequestHeap {
private:
    int _max_size;

    std::function<double(TileHeader&)> _priority;

    std::function<bool(TileHeader&, TileHeader&)> _compare;

    std::vector<TileHeader> _heap;

public:
    TileRequestHeap(int max_size);

    void push(TileHeader header);

    TileHeader pop();

    void rebuild(std::function<double(TileHeader&)> priority);

    bool contains(TileHeader header);

    void clear() { _heap.clear(); }

    unsigned int size() { return _heap.size(); }
};


#endif
