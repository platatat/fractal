#ifndef __SOCKET_UTIL_H__
#define __SOCKET_UTIL_H__

#include "tile_header.h"


class SocketUtil {
public:
    static void sendData(int sock, char* data, int size);

    static int receiveData(int sock, char* buffer, int size);

    static void sendHeaderPacket(int sock, std::shared_ptr<TileHeader> header);

    static std::unique_ptr<TileHeader> receiveHeaderPacket(int sock);
};


#endif
