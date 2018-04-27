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

    // Clear screen.
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    cairo_fill(cr);

    // Do viewport transformations.
    cairo_save(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_scale(cr, fractional_scale, fractional_scale);
    double translate_x = -viewportInfo.fractional_x * (Constants::TILE_WIDTH);
    double translate_y = -viewportInfo.fractional_y * (Constants::TILE_HEIGHT);
    cairo_translate(cr, translate_x, translate_y);

    // Compute stride for surface creation.
    cairo_format_t tile_format = CAIRO_FORMAT_A8;
    int tile_stride = cairo_format_stride_for_width(tile_format, Constants::TILE_WIDTH);

    int tx = 0;
    int ty = 0;

    for (Tile* tile : tiles) {
        if (tile != nullptr) {
            unsigned char* tile_buffer = tile->getData();

            cairo_surface_t* tile_surface = cairo_image_surface_create_for_data(
                    tile_buffer, tile_format, Constants::TILE_WIDTH,
                    Constants::TILE_HEIGHT, tile_stride);

            cairo_status_t tile_surface_status = cairo_surface_status(tile_surface);
            if (tile_surface_status != CAIRO_STATUS_SUCCESS) {
                std::cout << "ERROR: Surface creation failed: ";
                std::cout << cairo_status_to_string(tile_surface_status) << std::endl;
            }
            
            int screen_x = tx * Constants::TILE_WIDTH;
            int screen_y = ty * Constants::TILE_HEIGHT;

            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_mask_surface(cr, tile_surface, screen_x, screen_y);

            cairo_surface_destroy(tile_surface);
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


void Renderer::histogramColor(unsigned char* screen) {
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, Constants::SCREEN_WIDTH);
    int pixels = Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT;

    // TODO: why the hell can't I stack allocate these?
    int* pdf = new int [256];
    int* cdf = new int [256];

    // Initialize PDF and CDF to zero.
    for (int i = 0; i < 256; i++) {
        pdf[i] = 0;
        cdf[i] = 0;
    }

    // Compute PDF from red channel (can use any except alpha).
    for (int i = 0; i < pixels; i++) {
        unsigned char red   = screen[i * 4 + 0];
        unsigned char green = screen[i * 4 + 1];
        unsigned char blue  = screen[i * 4 + 2];
        unsigned char alpha = screen[i * 4 + 3];

        pdf[red] += 1;
    }

    // Compute CDF by integrating PDF.
    cdf[0] = pdf[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + pdf[i];
    }

    // Recolor pixels from CDF.
    for (int i = 0; i < pixels; i++) {
        unsigned char value = screen[i * 4];
        double color = (double) cdf[value] / pixels;

        unsigned char alpha = 255;
        unsigned char red   = color * 255;
        unsigned char green = color * 255;
        unsigned char blue  = color * 255;

        screen[4 * i + 3] = alpha;
        screen[4 * i + 2] = red;
        screen[4 * i + 1] = green;
        screen[4 * i + 0] = blue;
    }

    delete pdf;
    delete cdf;
}


const unsigned char* Renderer::getScreenBuffer() {
    unsigned char* screen_buffer = cairo_image_surface_get_data(pimpl->surface);
    histogramColor(screen_buffer);
    return screen_buffer;
}


int Renderer::getScreenBufferStride() {
    return cairo_image_surface_get_stride(pimpl->surface);
}


Renderer::~Renderer(){}
