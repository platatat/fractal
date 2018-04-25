#include "renderer.h"
#include <iostream>

#include <math.h>
#include <cairo.h>

class Renderer::Pimpl {
public:
    cairo_surface_t *surface;
    cairo_t *cr;
};

Renderer::Renderer() : _origin({0.0, 0.0}), _zoom(0) {
    pimpl.reset(new Pimpl());

    pimpl->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
                                                Constants::SCREEN_WIDTH,
                                                Constants::SCREEN_HEIGHT);
    pimpl->cr = cairo_create(pimpl->surface);

    _tile_manager = TileManager(64);
}


void Renderer::render() {
    cairo_t *cr = pimpl->cr;

    std::vector<Tile*> tiles;

    double screen_width = pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
    double screen_height = pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;
    complex screen_size = {screen_width, screen_height};

    _tile_manager.loadViewport(_origin, screen_size, (int) (_zoom), tiles);

    cairo_set_source_rgb(cr, 1.0, 0.7, 1.0);
    cairo_rectangle (cr, 0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    cairo_fill (cr);

    for (Tile* tile : tiles) {
        unsigned char* buffer = (unsigned char*) tile->getData();
        TileHeader header = tile->getHeader();
        complex tile_origin = tile->getOrigin();
        double tile_size = tile->getSize();

        double left = (tile_origin.real - _origin.real).toDouble() / screen_width;
        double right = (tile_origin.real + tile_size - _origin.real).toDouble() / screen_width;
        double bottom = (tile_origin.imag - _origin.imag).toDouble() / screen_height;
        double top = (tile_origin.imag + tile_size - _origin.imag).toDouble() / screen_height;

        double left_px = left * Constants::SCREEN_WIDTH;
        double top_px = (1 - top) * Constants::SCREEN_HEIGHT;

        double x_scale = (right - left) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
        double y_scale = (top - bottom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;

        std::cout << header.y << ", " << top_px << ", " << y_scale << "\n";

        cairo_save(cr);
        cairo_scale(cr, x_scale, y_scale);
        cairo_surface_t* imageSurface = cairo_image_surface_create_for_data(
                buffer, CAIRO_FORMAT_A8, Constants::TILE_WIDTH, 
                Constants::TILE_HEIGHT, Constants::TILE_WIDTH);
        cairo_set_source_surface (cr, imageSurface, left_px, top_px);
        cairo_paint(cr);
        cairo_restore(cr);

        cairo_surface_flush(pimpl->surface);
    }

    cairo_surface_flush(pimpl->surface); 
}

const unsigned char* Renderer::getScreenBuffer() {
    return cairo_image_surface_get_data(pimpl->surface);
}

int Renderer::getScreenBufferStride() {
    return cairo_image_surface_get_stride(pimpl->surface);
}

Renderer::~Renderer(){}
