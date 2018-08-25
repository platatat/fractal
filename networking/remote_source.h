#ifndef __REMOTE_SOURCE_H__
#define __REMOTE_SOURCE_H__

#include "tilestream/sink.h"
#include "tilestream/source.h"

#include <mutex>
#include <thread>

namespace networking {

class RemoteSource : public tilestream::Source {
public:
    RemoteSource(int socket_fd, std::shared_ptr<tilestream::Sink> tile_sink);

    bool request(std::shared_ptr<TileHeader> header);

private:
    int socket_fd;
    std::thread thread;

    std::mutex write_mutex;

    static void receiveTask(int socket_fd, std::shared_ptr<tilestream::Sink> tile_sink);
};

}

#endif
