#ifndef __SOCKET_UTIL_H__
#define __SOCKET_UTIL_H__

#include "solver/tile_header.h"

namespace networking {

void sendPacket(int sock, std::vector<uint8_t> data);
std::vector<uint8_t> receivePacket(int sock);

std::vector<uint8_t> serialize(std::vector<uint16_t> v);
std::vector<uint16_t> deserializeVector16(std::vector<uint8_t> data);

}

#endif
