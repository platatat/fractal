#include "constants.h"
#include "tile_client.h"
#include "tile_server.h"
#include "tile_solver.h"
#include <iostream>
#include <chrono>

#ifndef HPS
#include "application.h"

int runClient(std::string ip_addr) {
    Application app(ip_addr);
    app.init();
    app.run();
    return 0;
}
#else
int runClient() {
    printf("Client not supported on HPS build.\n");
    return 1;
}
#endif


int runServer() {
    TileServer server(Constants::PORT, 4);
    server.init();
    server.serveForever();
}


int main(int argc, char* args[])
{
    if (argc < 2) {
        std::cout << "must specify client or server" << std::endl;
        return -1;
    }

    try {
        if (strcmp(args[1], "client") == 0) {
            if (argc < 3) {
                std::cout << "must specify ip address" << std::endl;
                return -1;
            }
            std::string ip_addr = args[2];
            return runClient(ip_addr);
        }

        else if (strcmp(args[1], "server") == 0) {
            return runServer();
        }

        else {
            std::cout << "unrecognized command: " << args[1] << std::endl;
            return -1;
        }
    } 

    catch (std::runtime_error& e) {
        std::cout << "EXITING WITH EXCEPTION" << std::endl;
        std::cout << e.what() << std::endl;
        return -1;
    }
}
