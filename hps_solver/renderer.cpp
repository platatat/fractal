#include "renderer.h"
#include <iostream>

#include <math.h>
#include <cairo.h>


Renderer::Renderer() {
    int pixel_count = Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT;
    _data_buffer = new unsigned char [pixel_count];
    _screen_buffer = new unsigned char [pixel_count * 3];
}


void Renderer::render(const TileManager::ViewportInfo& viewportInfo,
                      const std::vector<std::shared_ptr<Tile>>& tiles,
                      double fractional_scale) {
    BFloat translate_x = -viewportInfo.fractional_x * (Constants::TILE_WIDTH);
    BFloat translate_y = -viewportInfo.fractional_y * (Constants::TILE_HEIGHT);

    int tx = 0;
    int ty = 0;

    for (std::shared_ptr<Tile> tile : tiles) {
        if (tile != nullptr) {
            unsigned char* tile_buffer = tile->getData();
            
            for (int y = 0; y < Constants::TILE_HEIGHT; y++) {
                for (int x = 0; x < Constants::TILE_WIDTH; x++) {
                    int screen_x = tx * Constants::TILE_WIDTH + x;
                    int screen_y = ty * Constants::TILE_HEIGHT + y;

                    // Apply viewport transformations.
                    screen_x = ((translate_x + screen_x) * fractional_scale).floor().toLong();
                    screen_y = ((translate_y + screen_y) * fractional_scale).floor().toLong();

                    // Draw pixels.
                    if (screen_x >= 0 && screen_x < Constants::SCREEN_WIDTH) {
                        if (screen_y >= 0 && screen_y < Constants::SCREEN_HEIGHT) {
                            unsigned char tile_value = tile->getPoint(x, y);
                            _data_buffer[screen_x + screen_y * Constants::SCREEN_WIDTH] = tile_value;
                        }
                    }
                }
            }
        }

        tx += 1;
        if (tx >= viewportInfo.tiles_width) {
            tx = 0;
            ty += 1;
        }
    }

    histogramColor();
}


void Renderer::histogramColor() {
    // TODO: why the hell can't I stack allocate these?
    int* pdf = new int [256];
    int* cdf = new int [256];

    // Initialize PDF and CDF to zero.
    for (int i = 0; i < 256; i++) {
        pdf[i] = 0;
        cdf[i] = 0;
    }

    // Compute PDF from red channel (can use any except alpha).
    for (int i = 0; i < Constants::SCREEN_PIXELS; i++) {
        unsigned char value = _data_buffer[i];
        pdf[value] += 1;
    }

    // Compute CDF by integrating PDF.
    cdf[0] = pdf[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + pdf[i];
    }

    // Recolor pixels from CDF.
    for (int i = 0; i < Constants::SCREEN_PIXELS; i++) {
        unsigned char value = _data_buffer[i];
        double color = (double) cdf[value] / Constants::SCREEN_PIXELS;

        unsigned char red   = color * 255;
        unsigned char green = color * 255;
        unsigned char blue  = color * 255;

        _screen_buffer[3 * i + 2] = red;
        _screen_buffer[3 * i + 1] = green;
        _screen_buffer[3 * i + 0] = blue;
    }

    delete[] pdf;
    delete[] cdf;
}

const unsigned char* Renderer::getScreenBuffer() {
    return _screen_buffer;
}


int Renderer::getScreenBufferStride() {
    return Constants::SCREEN_WIDTH * 3;
}


Renderer::~Renderer(){}
