#include "tile_client.h"
#include "constants.h"
#include "socket_util.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>


TileClient::TileClient(int port) : _port(port) {

}


void TileClient::init() {
    std::cout << "starting tile client on port " << _port << std::endl;

    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "socket error" << std::endl;
    }
  
    memset(&_server_address, '0', sizeof(_server_address));
  
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_port);
      
    if (inet_pton(AF_INET, "127.0.0.1", &_server_address.sin_addr) <= 0) {
        std::cout << "invalid address" << std::endl;
    }

    if (connect(_socket_fd, (sockaddr*) &_server_address, sizeof(_server_address)) < 0) {
        std::cout << "connect failed" << std::endl;
    }
}


void TileClient::requestTile(std::shared_ptr<TileHeader> header) {
    SocketUtil::sendHeaderPacket(_socket_fd, header);
}


std::unique_ptr<TileHeader> TileClient::receiveTile(unsigned char* buffer) {
    std::unique_ptr<TileHeader> header = SocketUtil::receiveHeaderPacket(_socket_fd);
    SocketUtil::receiveData(_socket_fd, buffer, Constants::TILE_PIXELS);
    return header;
}
