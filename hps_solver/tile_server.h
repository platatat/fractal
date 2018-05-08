#ifndef __TILE_SERVER_H__
#define __TILE_SERVER_H__

#include <sys/socket.h>
#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;


class TileServer {
private:
    int _port;
    int _socket_fd;
    sockaddr_in _address;
    socklen_t _address_len;

public:
    TileServer(int port);

    void init();

    void receive(int size, char* buffer);
};


#endif
