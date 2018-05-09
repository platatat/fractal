#include "application.h"
#include "constants.h"
#include "tile_client.h"
#include "tile_server.h"
#include "tile_solver.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>


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
        auto frame_start = std::chrono::high_resolution_clock::now();

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

        auto frame_end = std::chrono::high_resolution_clock::now();
        int frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start).count();
        int delay_milliseconds = std::max(0, 16667 - frame_duration) / 1000;

        // Sleep
        SDL_Delay(delay_milliseconds);

        double frames_per_second = 1000000.0 / frame_duration;
        // std::cout << "FPS: " << frames_per_second << std::endl;
    }

    // Clean up
    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();

    return 0;
}


void serverTest() {
    TileServer server(8080, 4);
    server.init();
    server.serveForever();
}


int socketTest(int argc, char* args[]) {
    if (argc < 2) std::cout << "must specify client or server" << std::endl;
    else if (strcmp(args[1], "client") == 0) runApp();
    else if (strcmp(args[1], "server") == 0) serverTest();
    else std::cout << "unrecognized: " << args[1] << std::endl;
}


int main(int argc, char* args[])
{
    try {
        // return runApp();
        return socketTest(argc, args);
        // return serializeTest();
    } catch (std::runtime_error& e) {
        std::cout << "EXITING WITH EXCEPTION" << std::endl;
        std::cout << e.what() << std::endl;
        return -1;
    }
}
