#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


class Application {
private:
    bool _running;
    complex _origin;
    double _zoom;
    double _fps;

    TileManager _tile_manager;
    Renderer _renderer;

    SDL_Window*     _window;
    SDL_Surface*    _window_surface;
    SDL_Renderer*   _sdl_renderer;

    TTF_Font*   _font_regular;
    TTF_Font*   _font_bold;

    SDL_Color   _clear_color;
    SDL_Color   _color_white;
    SDL_Color   _color_grey;

    void handleEvents();
    void handleInput();
    void drawFrame();
    void drawHUD();

    void setDrawColor(const SDL_Color& color);

    double getScreenWidth();
    double getScreenHeight();

public:
    Application(std::string ip_addr);

    void init();
    void run();
};


#endif
