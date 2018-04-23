#include "renderer.h"
#include <iostream>

#include <math.h>

#include <cairo.h>

class Renderer::Pimpl {
public:
    cairo_surface_t *surface;
    cairo_t *cr;
};

Renderer::Renderer() : _origin({0, 0}), _zoom(8) {
    //_screen_buffer = new char [Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT];

    pimpl.reset(new Pimpl());

    pimpl->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
                                                Constants::SCREEN_WIDTH,
                                                Constants::SCREEN_HEIGHT);
    pimpl->cr = cairo_create(pimpl->surface);
}


void Renderer::render() {
    //std::vector<Tile*> tiles;
    //_tile_manager.loadViewport(_origin, {1, 1}, 0, tiles);
    //_screen_buffer = tiles[0]->getData();



    // Dimensions of a tile in mandelbrot space at this zoom level.
    // double ms_tile_width = Constants::TILE_WIDTH * pow(2, -_zoom);
    // double ms_tile_height = Constants::TILE_HEIGHT * pow(2, -_zoom);

    // int tile_origin_x = (int) (_origin.real.toDouble() / ms_tile_width);
    // int tile_origin_y = (int) (_origin.imag.toDouble() / ms_tile_height);

    //_screen_buffer = _tile_manager.loadTile(0, 0, _zoom);

    cairo_t *cr = pimpl->cr;

    float angle = 0;//1.2f;

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20.0);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle (cr, 0, 0, 640, 480);
    cairo_fill (cr);

    cairo_save(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);

    cairo_translate(cr, 320, 240);
    cairo_rotate(cr, angle * (M_PI/180.));
    cairo_translate(cr, -100, 0);

    cairo_move_to(cr, 0, 0);
    cairo_text_path(cr, "do whatever makes you happy");
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 1, 0, 1);
    cairo_rectangle (cr, 0, 0, 10, 10);
    cairo_fill (cr);

    cairo_restore(cr);

    cairo_surface_flush(pimpl->surface);
}

const unsigned char* Renderer::getScreenBuffer() {
    return cairo_image_surface_get_data(pimpl->surface);
}

int Renderer::getScreenBufferStride() {
    return cairo_image_surface_get_stride(pimpl->surface);
}

Renderer::~Renderer(){}
