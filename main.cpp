#include "application.h"
#include "balancer.h"
#include "constants.h"
#include "tile_client.h"
#include "tile_server.h"
#include "worker.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

// TODO remove when networking gets factored out
#include "networking/remote_sink.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int runClient(std::vector<std::tuple<std::string, int>> ip_addrs) {
    Application app(ip_addrs);
    app.init();
    app.run();
    return 0;
}


void runServer(int port) {
    TileServer server(port);
    server.serveForever();
}


int main(int argc, char* args[])
{
    if (argc < 2) {
        std::cout << "must specify client, server, balancer, or worker" << std::endl;
        return -1;
    }

    try {
        if (strcmp(args[1], "client") == 0) {
            if (argc < 3) {
                std::cout << "must specify at least one ip address" << std::endl;
                return -1;
            }
            std::vector<std::tuple<std::string, int>> ip_addrs;
            for (int i = 2; i < argc; i++) {
                std::string arg = args[i];
                int index = arg.find(":");
                std::string ip_addr = arg.substr(0, index);
                int port = stoi(arg.substr(index + 1, arg.size() - index - 1));
                ip_addrs.emplace_back(ip_addr, port);
            }
            return runClient(ip_addrs);
        }

        else if (strcmp(args[1], "server") == 0) {
            if (argc < 3) {
                std::cout << "must specify a port" << std::endl;
                return -1;
            }
            std::stringstream ss(args[2]);
            int port;
            ss >> port;
            runServer(port);
        }

        else if (strcmp(args[1], "balancer") == 0) {
            if (argc < 5) {
                std::cout << "must specify worker_port client_port inflight_max" << std::endl;
                return -1;
            }
            std::stringstream worker_port_ss(args[2]);
            int worker_port;
            worker_port_ss >> worker_port;

            std::stringstream client_port_ss(args[3]);
            int client_port;
            client_port_ss >> client_port;

            std::stringstream inflight_max_ss(args[4]);
            int inflight_max;
            inflight_max_ss >> inflight_max;

            Balancer b(worker_port, inflight_max);
            b.serveForever(client_port);
        }

        else if (strcmp(args[1], "worker") == 0) {
            if (argc < 5) {
                std::cout << "must specify ip:port max_queue_size num_threads" << std::endl;
                return -1;
            }
            std::string arg = args[2];
            int index = arg.find(":");
            std::string ip_addr = arg.substr(0, index);
            int port = stoi(arg.substr(index + 1, arg.size() - index - 1));

            // TODO factor this out into networking
            int socket_fd;
            if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cout << "socket error" << std::endl;
            }
            struct sockaddr_in _server_address;
            memset(&_server_address, '0', sizeof(_server_address));
            _server_address.sin_family = AF_INET;
            _server_address.sin_port = htons(port);
            if (inet_pton(AF_INET, ip_addr.c_str(), &_server_address.sin_addr) <= 0) {
                std::cout << "invalid address" << std::endl;
            }
            if (connect(socket_fd, (sockaddr*) &_server_address, sizeof(_server_address)) < 0) {
                std::cout << "connect failed" << std::endl;
            }

            std::stringstream queue_size_ss(args[3]);
            int max_queue_size;
            queue_size_ss >> max_queue_size;

            std::stringstream num_threads_ss(args[4]);
            int num_threads;
            num_threads_ss >> num_threads;

            std::shared_ptr<Worker> worker(new Worker(max_queue_size));
            std::shared_ptr<networking::RemoteSink> remote_sink(new networking::RemoteSink(socket_fd, worker));
            worker->setSink(remote_sink);
            worker->run(num_threads);
        }

        else {
            std::cout << "unrecognized command: " << args[1] << std::endl;
            return -1;
        }
    } 

    catch (std::runtime_error& e) {
        std::cout << "EXITING WITH EXCEPTION" << std::endl;
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
