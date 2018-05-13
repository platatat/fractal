#ifndef __TILE_SERVER_H__
#define __TILE_SERVER_H__

#include "solver.h"
#include "tile_header.h"

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
    int _port;
    int _socket_fd;
    sockaddr_in _address;
    socklen_t _address_len;
    int _connection;

    int _queue_depth;
    std::set<std::shared_ptr<TileHeader>> _requests;
    std::mutex _mutex;
    std::condition_variable _requests_space_available;
    std::thread _tile_poll_thread;

    std::unique_ptr<Solver> solver;

    static void tilePollTask(TileServer* tile_server);

public:
    TileServer(int port, int queue_depth);

    void init();

    int awaitConnection();

    void serveForever();
};


#endif
