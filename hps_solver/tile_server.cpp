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

/**
 * COMMUNICATION PROTOCOL:
 * 0 -> Server initializes
 * 1 -> Server listens for tile request packet
 * 2 -> Client sends tile request packet
 * 3 -> Server recieves tile request packet
 * 4 -> Client listens for response packet
 * 5 -> Server generates the tile
 * 6 -> Server sends response packet
 * 7 -> Go to step 1
 */


TileServer::TileServer(int port) : _port(port) {
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
    std::unique_lock<std::mutex> lock(tile_server->_mutex);

    while(true) {
        tile_server->_requests_nonempty.wait(lock);

        while (tile_server->_requests.size() > 0) {
            std::shared_ptr<TileHeader> header = tile_server->_requests.front();
            tile_server->_requests.pop_front();

            lock.unlock();
            // TODO: don't need shared ptr shit here.
            // TODO: iterations (and maybe tile size) should be sent from client.
            unsigned char* tile_data = new unsigned char [Constants::TILE_PIXELS];
            std::shared_ptr<Tile> tile = std::make_shared<Tile>(header, tile_data);
            TileSolver::solveTile(tile, Constants::ITERATIONS);

            SocketUtil::sendHeaderPacket(tile_server->_connection, header);
            SocketUtil::sendData(tile_server->_connection, tile_data, Constants::TILE_PIXELS);

            lock.lock();
        }
    }
}


void TileServer::serveForever() {
    std::unique_lock<std::mutex> lock(_mutex);
    lock.unlock();

    _connection = awaitConnection();

    while (true) {
        try {
            std::shared_ptr<TileHeader> header = SocketUtil::receiveHeaderPacket(_connection);

            lock.lock();
            _requests.push_back(header);
            _requests_nonempty.notify_one();
            lock.unlock();
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            return;
        }
    }
}