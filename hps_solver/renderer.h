#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "tile_manager.h"

class Renderer {
private:
    uint8_t* _colored_buffer;

    unsigned int* _iterations_pdf;
    double* _iterations_cdf;

    void histogramColor();
    SDL_Color cyclicColor(uint8_t iterations);

public:
    Renderer();
    ~Renderer();

    void render(const TileManager::ViewportInfo& viewport_info,
                const std::vector<std::shared_ptr<Tile>>& tiles,
                int mipmap_level,
                double fractional_scale,
                SDL_Renderer* sdl_renderer);
};


#endif
