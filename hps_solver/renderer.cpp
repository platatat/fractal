#include "renderer.h"
#include <iostream>

#include <math.h>
#include <cairo.h>


Renderer::Renderer() {
    _data_buffer = new unsigned char [Constants::SCREEN_PIXELS];
    _screen_buffer = new unsigned char [Constants::SCREEN_PIXELS * 3];

    _iterations_pdf = new unsigned int [Constants::ITERATIONS];
    _iterations_cdf = new double [Constants::ITERATIONS];
}


Renderer::~Renderer() {
    delete[] _data_buffer;
    delete[] _screen_buffer;

    delete[] _iterations_pdf;
    delete[] _iterations_cdf;
}


void Renderer::render(const TileManager::ViewportInfo& viewport_info,
                      const std::vector<std::shared_ptr<Tile>>& tiles,
                      double fractional_scale) {
    // Clear PDF for coloring.
    for (int i = 0; i < Constants::ITERATIONS; i++) {
        _iterations_pdf[i] = 0;
    }

    double translate_x = -viewport_info.fractional_x * (Constants::TILE_WIDTH);
    double translate_y = -viewport_info.fractional_y * (Constants::TILE_HEIGHT);

    for (int tile_x = 0; tile_x < viewport_info.tiles_width; tile_x++) {
        for (int tile_y = 0; tile_y < viewport_info.tiles_height; tile_y++) {
            int tile_index = tile_y * viewport_info.tiles_width + tile_x;
            std::shared_ptr<Tile> tile = tiles[tile_index];

            int tile_screen_x = tile_x * Constants::TILE_WIDTH;
            int tile_screen_y = tile_y * Constants::TILE_HEIGHT;

            for (int y = 0; y < Constants::TILE_HEIGHT; y++) {
                for (int x = 0; x < Constants::TILE_WIDTH; x++) {
                    int screen_x = tile_screen_x + x;
                    int screen_y = tile_screen_y + y;

                    // Apply viewport transformations.
                    screen_x = std::floor((translate_x + screen_x) * fractional_scale);
                    screen_y = std::floor((translate_y + screen_y) * fractional_scale);

                    // Draw pixels.
                    if (screen_x >= 0 && screen_x < Constants::SCREEN_WIDTH) {
                        if (screen_y >= 0 && screen_y < Constants::SCREEN_HEIGHT) {
                            unsigned char tile_value = 0;

                            if (tile->hasData()) {
                                tile_value = tile->getPoint(x, y);
                                _iterations_pdf[tile_value] += 1;
                            }

                            _data_buffer[screen_x + screen_y * Constants::SCREEN_WIDTH] = tile_value;
                        }
                    }
                }
            }
        }
    }

    // Recompute CDF by normalizing and integrating new PDF.
    double pdf_sum = 0;
    for (int i = 1; i < Constants::ITERATIONS - 1; i++) {
        pdf_sum += _iterations_pdf[i];
    }

    _iterations_cdf[0] = _iterations_pdf[0] / pdf_sum;
    for (int i = 1; i < Constants::ITERATIONS; i++) {
        _iterations_cdf[i] = _iterations_cdf[i - 1] + (_iterations_pdf[i] / pdf_sum);
    }

    // histogramColor();
    cyclicColor();
}


void Renderer::histogramColor() {
    for (int i = 0; i < Constants::SCREEN_PIXELS; i++) {
        unsigned char value = _data_buffer[i];
        double color = _iterations_cdf[value];

        if (value == Constants::ITERATIONS - 1) {
            color = 1.0;
        }

        unsigned char red   = color * 255;
        unsigned char green = color * 255;
        unsigned char blue  = color * 255;

        _screen_buffer[3 * i + 2] = red;
        _screen_buffer[3 * i + 1] = green;
        _screen_buffer[3 * i + 0] = blue;
    }
}


void Renderer::cyclicColor() {
    double cycle_period = 5;

    for (int i = 0; i < Constants::SCREEN_PIXELS; i++) {
        unsigned char value = _data_buffer[i];
        double phase = value / cycle_period;

        unsigned char red   = 0;
        unsigned char green = (sin(phase) * 127) + 128;
        unsigned char blue  = (sin(phase) * 127) + 128;

        if (value == Constants::ITERATIONS - 1) {
            red     = 255;
            blue    = 255;
            green   = 255;
        }

        _screen_buffer[3 * i + 2] = red;
        _screen_buffer[3 * i + 1] = green;
        _screen_buffer[3 * i + 0] = blue;
    }
}


const unsigned char* Renderer::getScreenBuffer() {
    return _screen_buffer;
}


int Renderer::getScreenBufferStride() {
    return Constants::SCREEN_WIDTH * 3;
}