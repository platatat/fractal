#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


class Constants {
public:
    static constexpr const char* WINDOW_TITLE = "Mandelbrot Renderer";
    static const int TARGET_FPS         = 60;
    static const int PORT               = 8080;

    static const int SCREEN_WIDTH       = 512;
    static const int SCREEN_HEIGHT      = 512;

    static const int TILE_WIDTH         = 64;
    static const int TILE_HEIGHT        = 64;

    static const int ITERATIONS         = 16;
    static const int MPF_PRECISION      = 64;
    static const int CACHE_SIZE         = 1024;

    static const int SCREEN_PIXELS      = SCREEN_WIDTH * SCREEN_HEIGHT;
    static const int TILE_PIXELS        = TILE_WIDTH * TILE_HEIGHT;
};


#endif
