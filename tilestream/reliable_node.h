#ifndef __RELIABLE_NODE_H__
#define __RELIABLE_NODE_H__

#include "sink.h"
#include "source.h"

#include <condition_variable>
#include <mutex>
#include <unordered_set>

namespace tilestream {

class ReliableNode : public Source, public Sink {
public:
    ReliableNode(std::shared_ptr<Sink> sink, int inflight_max);

    void setSource(std::shared_ptr<Source> source);
    void setResendSource(std::shared_ptr<Source> source);

    bool request(std::shared_ptr<TileHeader> header);
    void receive(std::shared_ptr<Tile> tile);
    void close();

private:
    bool is_closed;
    std::shared_ptr<Sink> sink;
    std::shared_ptr<Source> source;
    std::shared_ptr<Source> resend_source;

    int inflight_max;
    std::mutex mutex;
    std::condition_variable has_inflight_space;
    std::unordered_set<std::shared_ptr<TileHeader>, TileHeader::Hasher, TileHeader::Comparator> inflight;
};

}

#endif
