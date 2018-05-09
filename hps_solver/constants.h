#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


class Constants {
public:
    static const int SCREEN_WIDTH       = 512;
    static const int SCREEN_HEIGHT      = 512;

    static const int TILE_WIDTH         = 128;
    static const int TILE_HEIGHT        = 128;

    static const int ITERATIONS         = 64;
    static const int MPF_PRECISION      = 64;

    static const int SCREEN_PIXELS      = SCREEN_WIDTH * SCREEN_HEIGHT;
    static const int TILE_PIXELS        = TILE_WIDTH * TILE_HEIGHT;

    static const int PORT               = 8080;
};


#endif
