#include "balancer.h"

#include "networking/remote_sink.h"
#include "networking/remote_source.h"

Balancer::Balancer(int worker_port) : cache(new tilestream::Cache), source_demux(new tilestream::SourceDemux), worker_server_thread(workerServerTask, this, worker_port) {
    this->cache->setSource(this->source_demux);
}

void Balancer::workerServerTask(Balancer* balancer, int port) {
    int socket_fd;
    sockaddr_in address;
    socklen_t address_len;

    address_len = sizeof(address);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed" << std::endl;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(socket_fd, (sockaddr*) &address, address_len) < 0) {
        std::cout << "worker port bind failed" << std::endl;
    }

    while (true) {
        int worker_socket_fd;
        if (listen(socket_fd, 3) < 0) {
            std::cout << "listen failed" << std::endl;
        }
        if ((worker_socket_fd = accept(socket_fd, (sockaddr*) &address, &address_len)) < 0) {
            std::cout << "worker accept failed" << std::endl;
        }
        std::cout << "new worker online" << std::endl;
        balancer->addWorker(worker_socket_fd);
    }
}

void Balancer::addWorker(int socket_fd) {
    std::shared_ptr<networking::RemoteSource> remote_source(new networking::RemoteSource(socket_fd, this->cache));
    this->source_demux->addSource(remote_source);
}

void Balancer::addClient(int socket_fd) {
    // TODO handle multiple clients better
    std::shared_ptr<networking::RemoteSink> remote_sink(new networking::RemoteSink(socket_fd, this->cache));
    this->cache->setSink(remote_sink); // TODO broken. This will cause old client to stop receiving tiles
}

void Balancer::serveForever(int client_port) {
    std::cout << "Started balancer" << std::endl;

    int socket_fd;
    sockaddr_in address;
    socklen_t address_len;

    address_len = sizeof(address);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed" << std::endl;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(client_port);
    if (bind(socket_fd, (sockaddr*) &address, address_len) < 0) {
        std::cout << "client port bind failed" << std::endl;
    }

    while (true) {
        int client_socket_fd;
        if (listen(socket_fd, 3) < 0) {
            std::cout << "listen failed" << std::endl;
        }
        if ((client_socket_fd = accept(socket_fd, (sockaddr*) &address, &address_len)) < 0) {
            std::cout << "client accept failed" << std::endl;
        }
        std::cout << "client connected" << std::endl;
        this->addClient(client_socket_fd);
    }
}
