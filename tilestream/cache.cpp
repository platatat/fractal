#include "cache.h"

namespace tilestream {

bool Cache::request(std::shared_ptr<TileHeader> header) {
    std::shared_ptr<Tile> tile = nullptr;
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        auto cache_lookup = data.find(header);
        if (cache_lookup != data.end()) {
            tile = cache_lookup->second;
        }
    }
    if (tile == nullptr) {
        return this->source->request(header);
    } else {
        this->sink->receive(tile);
        return true;
    }
}

void Cache::receive(std::shared_ptr<Tile> tile) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        // TODO limit cache size
        data[tile->getHeader()] = tile;
    }

    if (this->sink != nullptr) {
        this->sink->receive(tile);
    }
}

void Cache::setSource(std::shared_ptr<Source> source) {
    this->source = source;
}

void Cache::setSink(std::shared_ptr<Sink> sink) {
    this->sink = sink;
}

}
