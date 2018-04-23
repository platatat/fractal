#include "application.h"
#include "constants.h"

#include <SDL2/SDL.h>
#include <stdio.h>

int main( int argc, char* args[] )
{
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
        surf = SDL_CreateRGBSurfaceWithFormatFrom((void*) buffer,
                                                  Constants::SCREEN_WIDTH,
                                                  Constants::SCREEN_HEIGHT,
                                                  24,
                                                  app.getDisplayStride(),
                                                  SDL_PIXELFORMAT_RGB888);

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
    }

    // Clean up
    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();

    return 0;
}
