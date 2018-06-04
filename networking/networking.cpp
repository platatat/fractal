#include "networking/networking.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

namespace networking {


void sendData(int sock, uint8_t* data, int size) {
    send(sock, data, size, MSG_NOSIGNAL);
}


int receiveData(int sock, uint8_t* buffer, int size) {
    int bytes_read = 0;

    while (bytes_read < size) {
        int bytes_to_read = std::min(1024, size - bytes_read);
        int result = read(sock, buffer + bytes_read, bytes_to_read);

        if (result < 1) {
            return -1;
        }

        bytes_read += result;
    }

    return 0;
}


void sendPacket(int sock, std::vector<uint8_t> data) {
    uint32_t packet_size = data.size();
    uint8_t packet_header [4];
    ((uint32_t*) packet_header)[0] = htonl(packet_size);
    sendData(sock, packet_header, 4);
    sendData(sock, &data[0], packet_size);
}


std::vector<uint8_t> receivePacket(int sock) {
    // Receive packet header.
    uint8_t packet_header [4];
    int err = receiveData(sock, packet_header, 4);
    if (err == -1) return std::vector<uint8_t>();

    uint32_t packet_size = ntohl(((uint32_t*) packet_header)[0]);

    // Receive packet body.
    uint8_t packet_body [packet_size];
    receiveData(sock, packet_body, packet_size);
    std::vector<uint8_t> result(packet_body, packet_body + packet_size);
    return result;
}


std::vector<uint8_t> serialize(std::vector<uint16_t> data) {
    std::vector<uint8_t> bytes;
    uint8_t buffer[2];

    for (unsigned int i = 0; i < data.size(); i++) {
        ((uint16_t*) buffer)[0] = htons(data[i]);
        bytes.push_back(buffer[0]);
        bytes.push_back(buffer[1]);
    }

    return bytes;
}


std::vector<uint16_t> deserializeVector16(std::vector<uint8_t> bytes) {
    std::vector<uint16_t> data;

    for (unsigned int i = 0; i < bytes.size(); i += 2) {
        data.push_back(ntohs(*((uint16_t*) (&bytes[0] + i))));
    }

    return data;
}


}
