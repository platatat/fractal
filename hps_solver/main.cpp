#include "application.h"
#include "constants.h"
#include "tile_client.h"
#include "tile_server.h"
#include "tile_solver.h"
#include <SDL2/SDL.h>
#include <iostream>


int runApp() {
    Application app;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a window
    SDL_Window* window;
    window = SDL_CreateWindow("mandelbrot",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN );

    if (window == nullptr) {
        printf("Window creation failed\n");
        return -1;
    }


    bool exit = false;
    while (!exit) {
        // Draw
        const unsigned char* buffer = app.display();

        SDL_Surface* surf;
        surf = SDL_CreateRGBSurfaceFrom((void*) buffer,
                                        Constants::SCREEN_WIDTH,
                                        Constants::SCREEN_HEIGHT,
                                        24, 
                                        app.getDisplayStride(),
                                        0, 0, 0, 0);

        SDL_Surface* win_surf = SDL_GetWindowSurface(window);
        SDL_BlitSurface(surf, NULL, win_surf, NULL);
        SDL_UpdateWindowSurface(window);

        SDL_FreeSurface(surf);

        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit = true;
            }
        }

        // Sleep
        SDL_Delay(16);
    }

    // Clean up
    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();

    return 0;
}


void clientTest() {
    TileClient client = TileClient(8080);
    client.run();
}


void serverTest() {
    TileServer server = TileServer(8080);
    server.init();

    char* data = new char[1024];

    server.receive(32, data);
    std::cout << data << std::endl;
}


int socketTest(int argc, char* args[]) {
    if (argc < 2) std::cout << "must specify client or server" << std::endl;
    else if (strcmp(args[1], "client") == 0) clientTest();
    else if (strcmp(args[1], "server") == 0) serverTest();
    else std::cout << "unrecognized: " << args[1] << std::endl;
}


int main(int argc, char* args[])
{
    // return runApp();
    return socketTest(argc, args);
}
