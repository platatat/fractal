#include "reliable_node.h"

namespace tilestream {

ReliableNode::ReliableNode(std::shared_ptr<Sink> sink) : is_closed(false), sink(sink){

}

void ReliableNode::close() {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->is_closed = true;
        for (std::shared_ptr<TileHeader> header : this->inflight) {
            this->resend_source->request(header);
        }
    }
}

bool ReliableNode::request(std::shared_ptr<TileHeader> header) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (this->is_closed) return false;

        this->inflight.insert(header);
    }

    this->source->request(header);
    return true;
}

void ReliableNode::receive(std::shared_ptr<Tile> tile) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->inflight.erase(tile->getHeader());
    }

    this->sink->receive(tile);
}

void ReliableNode::setSource(std::shared_ptr<Source> source) {
    this->source = source;
}

void ReliableNode::setResendSource(std::shared_ptr<Source> source) {
    this->resend_source = source;
}

}
