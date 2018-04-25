#include "renderer.h"
#include <iostream>

#include <math.h>
#include <cairo.h>

class Renderer::Pimpl {
public:
    cairo_surface_t *surface;
    cairo_t *cr;
};


Renderer::Renderer() {
    pimpl.reset(new Pimpl());

    pimpl->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
                                                Constants::SCREEN_WIDTH,
                                                Constants::SCREEN_HEIGHT);
    pimpl->cr = cairo_create(pimpl->surface);
}


void Renderer::render(const TileManager::ViewportInfo& viewportInfo,
                      const std::vector<Tile*>& tiles,
                      double fractional_scale) {
    cairo_t *cr = pimpl->cr;

    //printf("w: %i\th: %i\ts: %i\n", viewportInfo.tiles_width, viewportInfo.tiles_height, tiles.size());

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle (cr, 0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    cairo_fill (cr);

    cairo_save(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_ADD);

    cairo_scale(cr, fractional_scale, fractional_scale);

    cairo_translate(cr,
                    -viewportInfo.fractional_x * Constants::TILE_WIDTH,
                    -viewportInfo.fractional_y * Constants::TILE_HEIGHT);

    int tx = 0;
    int ty = 0;
    for (Tile* tile : tiles) {
        if (tile != nullptr) {
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
        }

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
