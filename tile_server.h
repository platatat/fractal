#ifndef __TILE_SERVER_H__
#define __TILE_SERVER_H__

#include "solver/solver.h"
#include "solver/tile_header.h"

#include <condition_variable>
#include <deque>
#include <mutex>
#include <netinet/in.h>
#include <set>
#include <sys/socket.h>
#include <thread>

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;


class TileServer {
private:
    int port;
    int socket_fd;
    sockaddr_in address;
    socklen_t address_len;

    std::vector<std::thread> client_listeners;

    std::set<std::tuple<std::shared_ptr<TileHeader>, int>> requests;
    std::mutex mutex;

    static void clientListenerTask(int connection);

public:
    TileServer(int port);

    void init();

    int awaitConnection();

    void serveForever();
};


#endif
