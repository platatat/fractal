#include "renderer.h"
#include <iostream>

#include <math.h>


Renderer::Renderer() {
    _colored_buffer = new uint8_t [Constants::TILE_PIXELS * 3];

    _iterations_pdf = new unsigned int [Constants::ITERATIONS];
    _iterations_cdf = new double [Constants::ITERATIONS];
}


Renderer::~Renderer() {
    delete[] _colored_buffer;

    delete[] _iterations_pdf;
    delete[] _iterations_cdf;
}


void Renderer::render(const std::set<std::shared_ptr<Tile>>& tiles, Viewport viewport, SDL_Renderer* sdl_renderer) {
    // Clear PDF for coloring.
    for (int i = 0; i < Constants::ITERATIONS; i++) {
        _iterations_pdf[i] = 0;
    }

    double translate_x = -viewport.partial_x * (Constants::TILE_WIDTH);
    double translate_y = -viewport.partial_y * (Constants::TILE_HEIGHT);

    for (std::shared_ptr<Tile> tile : tiles) {
        std::shared_ptr<TileHeader> header = tile->getHeader();

        mpz_class mp_tile_x = header->x - viewport.origin_x;
        mpz_class mp_tile_y = header->y - viewport.origin_y;
        int tile_x = mp_tile_x.get_si();
        int tile_y = mp_tile_y.get_si();

        double tile_screen_x = (translate_x + tile_x * Constants::TILE_WIDTH) * viewport.partial_zoom;
        double tile_screen_y = (translate_y + tile_y * Constants::TILE_HEIGHT) * viewport.partial_zoom;

        if (tile->hasData()) {
            std::vector<uint16_t> tile_data = tile->getData();

            for (int i = 0; i < Constants::TILE_PIXELS; i++) {
                SDL_Color color = cyclicColor(tile_data[i], header->iter_lim);
                _colored_buffer[i * 3 + 0] = color.b;
                _colored_buffer[i * 3 + 1] = color.g;
                _colored_buffer[i * 3 + 2] = color.r;
            }

            SDL_Surface* tile_surface = SDL_CreateRGBSurfaceFrom(
                (void*) _colored_buffer,
                Constants::TILE_WIDTH,
                Constants::TILE_HEIGHT,
                24,
                Constants::TILE_WIDTH * 3,
                0, 0, 0, 0
            );

            SDL_Texture* tile_texture = SDL_CreateTextureFromSurface(sdl_renderer, tile_surface);

            SDL_Rect dst_rect;
            dst_rect.x = std::floor(tile_screen_x);
            dst_rect.y = std::floor(tile_screen_y);
            dst_rect.w = std::ceil(Constants::TILE_WIDTH * viewport.partial_zoom);
            dst_rect.h = std::ceil(Constants::TILE_HEIGHT * viewport.partial_zoom);

            SDL_RenderCopy(sdl_renderer, tile_texture, NULL, &dst_rect);

            SDL_DestroyTexture(tile_texture);
            SDL_FreeSurface(tile_surface);
        }
    }

    // Recompute CDF by normalizing and integrating new PDF.
    // double pdf_sum = 0;
    // for (int i = 1; i < Constants::ITERATIONS - 1; i++) {
    //     pdf_sum += _iterations_pdf[i];
    // }

    // _iterations_cdf[0] = _iterations_pdf[0] / pdf_sum;
    // for (int i = 1; i < Constants::ITERATIONS; i++) {
    //     _iterations_cdf[i] = _iterations_cdf[i - 1] + (_iterations_pdf[i] / pdf_sum);
    // }

    // histogramColor();
    // cyclicColor();
}


// void Renderer::histogramColor() {
//     for (int i = 0; i < Constants::SCREEN_PIXELS; i++) {
//         unsigned char value = _data_buffer[i];
//         double color = _iterations_cdf[value];

//         if (value == Constants::ITERATIONS - 1) {
//             color = 1.0;
//         }

//         unsigned char red   = color * 255;
//         unsigned char green = color * 255;
//         unsigned char blue  = color * 255;

//         _screen_buffer[3 * i + 2] = red;
//         _screen_buffer[3 * i + 1] = green;
//         _screen_buffer[3 * i + 0] = blue;
//     }
// }


SDL_Color Renderer::cyclicColor(int16_t iterations, int16_t iter_lim) {
    double cycle_period = 5;
    double phase = iterations / cycle_period;

    SDL_Color color;

    color.r = 0;
    color.g = (sin(phase) * 127) + 128;
    color.b = (sin(phase) * 127) + 128;

    if (iterations == iter_lim - 1) {
        color.r = 255;
        color.g = 255;
        color.b = 255;
    }

    return color;
}
