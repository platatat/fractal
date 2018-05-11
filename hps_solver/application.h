#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "renderer.h"
#include <SDL2/SDL.h>


class Application {
private:
    bool _running;
    complex _origin;
    double _zoom;

    TileManager _tile_manager;
    Renderer _renderer;

    SDL_Window*     _window;
    SDL_Surface*    _window_surface;
    SDL_Renderer*   _sdl_renderer;

    void handleEvents();
    void handleInput();
    void display();

    double getScreenWidth();
    double getScreenHeight();

public:
    Application();

    void init();
    void run();
};


#endif
