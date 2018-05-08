#include "tile_server.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

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


void TileServer::receive(int size, char* buffer) {
    int socket;

    if (listen(_socket_fd, 3) < 0) {
        std::cout << "listen failed" << std::endl;
    }

    if ((socket = accept(_socket_fd, (sockaddr*) &_address, &_address_len)) < 0) {
        std::cout << "accept failed" << std::endl;
    }

    int bytes_read = 0;

    while (bytes_read < size) {
        int bytes_to_read = std::min(1024, size - bytes_read);
        int result = read(socket, buffer + bytes_read, bytes_to_read);
        if (result < 1) std::cout << "read failed" << std::endl;
        bytes_read += result;
    }
}