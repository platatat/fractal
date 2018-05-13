#ifndef __TILE_SERVER_H__
#define __TILE_SERVER_H__

#include "tile_header.h"
#include "fpga_solver.h"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
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
    int _connection;

    int _queue_depth;
    std::deque<std::shared_ptr<TileHeader>> _requests;
    std::mutex _mutex;
    std::condition_variable _requests_nonempty;
    std::condition_variable _requests_space_available;
    std::thread _tile_generation_thread;

    FPGASolver solver;

    static void tileGenerationTask(TileServer* tile_server);

public:
    TileServer(int port, int queue_depth);

    void init();

    int awaitConnection();

    void serveForever();
};


#endif
