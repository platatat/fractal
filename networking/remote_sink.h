#ifndef __REMOTE_SINK_H__
#define __REMOTE_SINK_H__

#include "tilestream/sink.h"
#include "tilestream/source.h"

#include <mutex>
#include <thread>

namespace networking {

class RemoteSink : public tilestream::Sink {
public:
    RemoteSink(int socket_fd, std::shared_ptr<tilestream::Source> tile_source);

    void receive(std::shared_ptr<Tile> tile);

private:
    int socket_fd;
    std::thread thread;

    std::mutex write_mutex;

    //metrics
    bool display_metrics;
    int num_requests;
    int num_tiles;
    std::chrono::time_point<std::chrono::system_clock> last_report;

    static void receiveTask(RemoteSink* remote_sink, int socket_fd, std::shared_ptr<tilestream::Source> tile_source);

    void incMetrics(int requests, int tiles);
};

}

#endif
