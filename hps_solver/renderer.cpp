#include "renderer.h"
#include <iostream>

#include <math.h>

#include <cairo.h>

class Renderer::Pimpl {
public:
    cairo_surface_t *surface;
    cairo_t *cr;
};

Renderer::Renderer() : _origin({0, 0}), _zoom(2) {
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

    auto viewportInfo = _tile_manager.loadViewport(_origin, screen_size, (int) (_zoom), tiles);

    //printf("w: %i\th: %i\ts: %i\n", viewportInfo.tiles_width, viewportInfo.tiles_height, tiles.size());

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle (cr, 0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    cairo_fill (cr);

    cairo_save(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
    cairo_scale(cr, 0.8, 0.8);

    int tx = 0;
    int ty = 0;
    for (Tile* tile : tiles) {
        unsigned char* buffer = (unsigned char*) tile->getData();

        cairo_surface_t* imageSurface = cairo_image_surface_create_for_data(
                buffer, CAIRO_FORMAT_A8, Constants::TILE_WIDTH, 
                Constants::TILE_HEIGHT, Constants::TILE_WIDTH);

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_mask_surface(cr, imageSurface,
                           tx * Constants::TILE_WIDTH,
                           ty * Constants::TILE_HEIGHT);

        /*
        cairo_set_source_surface (cr, imageSurface,
                                  tx * Constants::TILE_WIDTH,
                                  ty * Constants::TILE_HEIGHT);
        cairo_paint(cr);
        */

        cairo_surface_destroy(imageSurface);

        tx += 1;
        if (tx >= viewportInfo.tiles_width) {
            tx = 0;
            ty += 1;
        }
    }

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
