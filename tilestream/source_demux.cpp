#include "source_demux.h"

namespace tilestream {

//TODO when there are no sources or they're all bloked, make this block on first request - not the second
bool SourceDemux::request(std::shared_ptr<TileHeader> header) {
    {
        std::unique_lock<std::mutex> lock(this->request_mutex);
        while (this->waiting_request != nullptr) {
            this->has_request_space.wait(lock);
        }
        this->waiting_request = header;
        this->has_request.notify_all();
    }

    return true;
}

void SourceDemux::addSource(std::shared_ptr<Source> source) {
    {
        std::unique_lock<std::mutex> lock(this->source_threads_mutex);
        this->requesting_threads[source] = std::thread(requestingTask, this, source);
    }
}

void SourceDemux::requestingTask(SourceDemux* demux, std::shared_ptr<Source> source) {
    while (true) {
        std::shared_ptr<TileHeader> header;
        {
            std::unique_lock<std::mutex> lock(demux->request_mutex);
            while (demux->waiting_request == nullptr) {
                demux->has_request.wait(lock);
            }
            header = demux->waiting_request;
            demux->waiting_request = nullptr;
            demux->has_request_space.notify_all();
        }
        bool success = source->request(header);
        if (!success) {
            demux->request(header); // Rerequest the failed request attempt
            break;
        }
    }

    {
        std::unique_lock<std::mutex> lock(demux->source_threads_mutex);
        demux->requesting_threads.erase(source);
    }
}

}
