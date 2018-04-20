#include "renderer.h"


Renderer::Renderer() : _origin({0, 0}), _zoom(8) {
    _screen_buffer = new char [Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT];
}


void Renderer::render() {
    // Dimensions of a tile in mandelbrot space at this zoom level.
    // double ms_tile_width = Constants::TILE_WIDTH * pow(2, -_zoom);
    // double ms_tile_height = Constants::TILE_HEIGHT * pow(2, -_zoom);

    // int tile_origin_x = (int) (_origin.real.toDouble() / ms_tile_width);
    // int tile_origin_y = (int) (_origin.imag.toDouble() / ms_tile_height);



    _screen_buffer = _tile_manager.loadTile(0, 0, _zoom);
}