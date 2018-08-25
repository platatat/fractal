#ifndef __SOURCE_DEMUX_H__
#define __SOURCE_DEMUX_H__

#include "source.h"

#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

namespace tilestream {

// Souce demultiplexer
// Distributes requests to many sources
class SourceDemux : public Source {
public:
    void addSource(std::shared_ptr<Source> source);

    bool request(std::shared_ptr<TileHeader> header);

private:
    std::mutex request_mutex;
    std::condition_variable has_request;
    std::condition_variable has_request_space;
    std::shared_ptr<TileHeader> waiting_request;

    std::mutex source_threads_mutex;
    std::map<std::shared_ptr<Source>, std::thread> requesting_threads;

    static void requestingTask(SourceDemux* demux, std::shared_ptr<Source> source);
};

}

#endif
