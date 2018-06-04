#include "tile_client.h"
#include "constants.h"
#include "networking/networking.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>


TileClient::TileClient(std::string ip_addr, int port) : ip_addr(ip_addr), _port(port) {

}


void TileClient::init() {
    std::cout << "starting tile client on port " << _port << std::endl;

    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "socket error" << std::endl;
    }
  
    memset(&_server_address, '0', sizeof(_server_address));
  
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_port);
      
    if (inet_pton(AF_INET, ip_addr.c_str(), &_server_address.sin_addr) <= 0) {
        std::cout << "invalid address" << std::endl;
    }

    if (connect(_socket_fd, (sockaddr*) &_server_address, sizeof(_server_address)) < 0) {
        std::cout << "connect failed" << std::endl;
    }
}


void TileClient::requestTile(std::shared_ptr<TileHeader> header) {
    std::vector<uint8_t> data = header->serialize();
    networking::sendPacket(_socket_fd, data);
}


std::unique_ptr<Tile> TileClient::receiveTile() {
    std::vector<uint8_t> header_data = networking::receivePacket(_socket_fd);
    std::shared_ptr<TileHeader> header = TileHeader::deserialize(header_data);
    
    std::vector<uint8_t> tile_bytes = networking::receivePacket(_socket_fd);
    std::vector<uint16_t> tile_data = networking::deserializeVector16(tile_bytes);

    return std::unique_ptr<Tile>(new Tile(header, tile_data));
}
