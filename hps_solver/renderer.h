#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "tile_manager.h"
#include "viewport.h"
#include <memory>
#include <set>
#include <SDL2/SDL.h>


class Renderer {
private:
    uint8_t* _colored_buffer;

    unsigned int* _iterations_pdf;
    double* _iterations_cdf;

    void histogramColor();
    SDL_Color cyclicColor(int16_t iterations, int16_t iter_lim);

public:
    Renderer();
    ~Renderer();

    void render(const std::set<std::shared_ptr<Tile>>& tiles,
                Viewport viewport,
                SDL_Renderer* sdl_renderer);
};


#endif
