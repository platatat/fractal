#ifndef __TILE_CLIENT_H__
#define __TILE_CLIENT_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


class TileClient {
private:
    int _port;

public:
    TileClient(int port);

    void run();
};


#endif
