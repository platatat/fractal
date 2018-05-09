#include "tile_server.h"
#include "constants.h"
#include "socket_util.h"
#include "tile.h"
#include "tile_solver.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <memory>


TileServer::TileServer(int port, int queue_depth) : _port(port), _queue_depth(queue_depth) {
    _tile_generation_thread = std::thread(tileGenerationTask, this);
}


void TileServer::init() {
    std::cout << "starting tile server on port " << _port << std::endl;

    _address_len = sizeof(_address);
      
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed" << std::endl;
    }

    int opt = 1;
      
    if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "setsockopt failed" << std::endl;
    }

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);
      
    if (bind(_socket_fd, (sockaddr*) &_address, _address_len) < 0) {
        std::cout << "bind failed" << std::endl;
    }
}


int TileServer::awaitConnection() {
    int socket;

    if (listen(_socket_fd, 3) < 0) {
        std::cout << "listen failed" << std::endl;
    }

    if ((socket = accept(_socket_fd, (sockaddr*) &_address, &_address_len)) < 0) {
        std::cout << "accept failed" << std::endl;
    }

    return socket;
}


void TileServer::tileGenerationTask(TileServer* tile_server) {
    while(true) {
        std::shared_ptr<TileHeader> header;

        {
            std::unique_lock<std::mutex> lock(tile_server->_mutex);

            while (tile_server->_requests.size() == 0) {
                tile_server->_requests_nonempty.wait(lock);
            }

            header = tile_server->_requests.front();
            tile_server->_requests.pop_front();
            tile_server->_requests_space_available.notify_one();
        }
        
        // TODO: iterations (and maybe tile size) should be sent from client.
        std::vector<uint8_t> tile_data = TileSolver::solveTile(header, Constants::ITERATIONS);
        std::vector<uint8_t> header_data = header->serialize();

        SocketUtil::sendPacket(tile_server->_connection, header_data);
        SocketUtil::sendPacket(tile_server->_connection, tile_data);
    }
}


void TileServer::serveForever() {
    _connection = awaitConnection();

    while (true) {
        try {
            std::vector<uint8_t> header_data = SocketUtil::receivePacket(_connection);
            std::unique_ptr<TileHeader> unique_header = TileHeader::deserialize(header_data);
            std::shared_ptr<TileHeader> header = std::move(unique_header);

            {
                std::unique_lock<std::mutex> lock(_mutex);

                while (_requests.size() >= _queue_depth) {
                    _requests_space_available.wait(lock);
                }

                _requests.push_back(header);
                _requests_nonempty.notify_one();
            }
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            return;
        }
    }
}