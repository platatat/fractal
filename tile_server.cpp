#include "tile_server.h"

#include "constants.h"
#include "networking/networking.h"
#include "solver/tile.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


TileServer::TileServer(int port) : port(port) {
    std::cout << "starting tile server on port " << port << std::endl;

    address_len = sizeof(address);
      
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed" << std::endl;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
      
    if (bind(socket_fd, (sockaddr*) &address, address_len) < 0) {
        std::cout << "bind failed" << std::endl;
    }
}


int TileServer::awaitConnection() {
    int socket;

    if (listen(socket_fd, 3) < 0) {
        std::cout << "listen failed" << std::endl;
    }

    if ((socket = accept(socket_fd, (sockaddr*) &address, &address_len)) < 0) {
        std::cout << "accept failed" << std::endl;
    }

    return socket;
}


void TileServer::clientListenerTask(int connection) {
    while (true) {
        std::vector<uint8_t> header_data = networking::receivePacket(connection);
        if (header_data.size() == 0) return;

        std::shared_ptr<TileHeader> header = TileHeader::deserialize(header_data);
        std::vector<uint16_t> tile_data = Solver::solveTile(header);
        std::vector<uint8_t> tile_bytes = networking::serialize(tile_data);

        networking::sendPacket(connection, header_data);
        networking::sendPacket(connection, tile_bytes);
    }
}


void TileServer::serveForever() {
    while (true) {
        int connection = awaitConnection();
        client_listeners.emplace_back(clientListenerTask, connection);
    }
}
