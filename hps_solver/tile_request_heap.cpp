#include "tile_request_heap.h"
#include <algorithm>


TileRequestHeap::TileRequestHeap(int max_size) {
    _max_size = max_size;

    _priority = [](std::shared_ptr<TileHeader> header) { 
        return 0; 
    };

    _compare = [this](std::shared_ptr<TileHeader> a, std::shared_ptr<TileHeader> b) {
        return _priority(a) > _priority(b);
    };
}


void TileRequestHeap::push(std::shared_ptr<TileHeader> header) {
    _heap.push_back(header);
    std::push_heap(_heap.begin(), _heap.end(), _compare);
}


std::shared_ptr<TileHeader> TileRequestHeap::pop() {
    std::shared_ptr<TileHeader> header = _heap.front();
    std::pop_heap(_heap.begin(), _heap.end(), _compare);
    _heap.pop_back();
    return header;
}


void TileRequestHeap::rebuild(std::function<double(std::shared_ptr<TileHeader>)> priority) {
    _priority = priority;
    std::make_heap(_heap.begin(), _heap.end(), _compare);
}


bool TileRequestHeap::contains(std::shared_ptr<TileHeader> header) {
    auto find_result = std::find(_heap.begin(), _heap.end(), header);
    return find_result != _heap.end();
}