#ifndef __BALANCER_H__
#define __BALANCER_H__

#include "tilestream/cache.h"
#include "tilestream/source_demux.h"

class Balancer {
public:
    Balancer(int worker_port);

    void serveForever(int client_port);

    void addClient(int socket_fd);
    void addWorker(int socket_fd);

private:
    std::shared_ptr<tilestream::Cache> cache;
    std::shared_ptr<tilestream::SourceDemux> source_demux;

    std::thread worker_server_thread;

    static void workerServerTask(Balancer* balancer, int port);
};

#endif
