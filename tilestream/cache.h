#ifndef __CACHE_H__
#define __CACHE_H__

#include "sink.h"
#include "source.h"

#include <mutex>
#include <unordered_map>

namespace tilestream {

class Cache : public Source, public Sink {
public:
    bool request(std::shared_ptr<TileHeader> header);
    void receive(std::shared_ptr<Tile> tile);

    void setSource(std::shared_ptr<Source> source);
    void setSink(std::shared_ptr<Sink> sink);

private:
    std::mutex mutex;
    std::unordered_map<std::shared_ptr<TileHeader>, std::shared_ptr<Tile>, TileHeader::Hasher, TileHeader::Comparator> data;

    std::shared_ptr<Source> source;
    std::shared_ptr<Sink> sink;
};

}

#endif
