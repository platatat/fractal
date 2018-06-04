#include "application.h"
#include "constants.h"
#include "tile_client.h"
#include "tile_server.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>


int runClient(std::vector<std::tuple<std::string, int>> ip_addrs) {
    Application app(ip_addrs);
    app.init();
    app.run();
    return 0;
}


void runServer(int port) {
    TileServer server(port);
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
                std::cout << "must specify at least one ip address" << std::endl;
                return -1;
            }
            std::vector<std::tuple<std::string, int>> ip_addrs;
            for (int i = 2; i < argc; i++) {
                std::string arg = args[i];
                int index = arg.find(":");
                std::string ip_addr = arg.substr(0, index);
                int port = stoi(arg.substr(index + 1, arg.size() - index - 1));
                ip_addrs.emplace_back(ip_addr, port);
            }
            return runClient(ip_addrs);
        }

        else if (strcmp(args[1], "server") == 0) {
            if (argc < 3) {
                std::cout << "must specify a port" << std::endl;
                return -1;
            }
            std::stringstream ss(args[2]);
            int port;
            ss >> port;
            runServer(port);
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

    return 0;
}
