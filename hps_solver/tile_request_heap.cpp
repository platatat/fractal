#include "tile_request_heap.h"
#include <algorithm>


TileRequestHeap::TileRequestHeap(int max_size) {
    _max_size = max_size;

    _priority = [](TileHeader& header) { 
        return 0; 
    };

    _compare = [this](TileHeader& a, TileHeader& b) {
        return _priority(a) > _priority(b);
    };
}


void TileRequestHeap::push(TileHeader header) {
    _heap.push_back(header);
    std::push_heap(_heap.begin(), _heap.end(), _compare);
}


TileHeader TileRequestHeap::pop() {
    TileHeader header = _heap.front();
    std::pop_heap(_heap.begin(), _heap.end(), _compare);
    _heap.pop_back();
    return header;
}


void TileRequestHeap::rebuild(std::function<double(TileHeader&)> priority) {
    _priority = priority;
    std::make_heap(_heap.begin(), _heap.end(), _compare);
}


bool TileRequestHeap::contains(TileHeader header) {
    auto find_result = std::find(_heap.begin(), _heap.end(), header);
    return find_result != _heap.end();
}