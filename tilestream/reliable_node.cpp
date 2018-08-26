#include "reliable_node.h"

namespace tilestream {

ReliableNode::ReliableNode(std::shared_ptr<Sink> sink, int inflight_max) : is_closed(false), sink(sink), inflight_max(inflight_max) {

}

void ReliableNode::close() {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (this->is_closed) return;

        std::cout << "Closing channel" << std::endl;

        this->is_closed = true;
        for (std::shared_ptr<TileHeader> header : this->inflight) {
            this->resend_source->request(header);
        }

        std::cout << "Channel closed. Resent " << (int) this->inflight.size() << " requests" << std::endl;
    }
}

bool ReliableNode::request(std::shared_ptr<TileHeader> header) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (this->is_closed) return false;

        while ((int) inflight.size() >= inflight_max) {
            this->has_inflight_space.wait(lock);
        }
        this->inflight.insert(header);
    }

    this->source->request(header);
    return true;
}

void ReliableNode::receive(std::shared_ptr<Tile> tile) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->inflight.erase(tile->getHeader());
        this->has_inflight_space.notify_all();
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
