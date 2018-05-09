#include "socket_util.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>


void SocketUtil::sendData(int sock, char* data, int size) {
    send(sock, data, size, 0);
}


int SocketUtil::receiveData(int sock, char* buffer, int size) {
    int bytes_read = 0;

    while (bytes_read < size) {
        int bytes_to_read = std::min(1024, size - bytes_read);
        int result = read(sock, buffer + bytes_read, bytes_to_read);

        if (result < 1) {
            std::cout << "read failed" << std::endl;
            return -1;
        }

        bytes_read += result;
    }

    return 0;
}


void SocketUtil::sendHeaderPacket(int sock, std::shared_ptr<TileHeader> header) {
    // std::cout << "sending: " << header.get_str() << std::endl;
    int header_size;
    char* header_data = header->serialize(header_size);
    sendData(sock, header_data, header_size);
    delete[] header_data;
}


std::unique_ptr<TileHeader> SocketUtil::receiveHeaderPacket(int sock) {
    // Receive packet size data.
    char size_buffer [8]; 
    
    if (receiveData(sock, size_buffer, 8) != 0) {
        throw std::runtime_error("failed to receive tile header size");
    }

    // Calculate total packet size.
    int x_size = ((int*) size_buffer)[0];
    int y_size = ((int*) size_buffer)[1];
    int packet_size = x_size + y_size + 16;

    // Recieve the rest of the packet.
    char packet_buffer [packet_size];
    std::memcpy(packet_buffer, size_buffer, 8);

    if (receiveData(sock, packet_buffer + 8, packet_size - 8) != 0) {
        throw std::runtime_error("failed to receive tile header data");
    }

    // Deserialize.
    std::unique_ptr<TileHeader> header = TileHeader::deserialize(packet_size, packet_buffer);
    // std::cout << "received: " << header.get_str() << std::endl;
    return header;
}