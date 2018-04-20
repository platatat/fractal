#ifndef __TILE_H__
#define __TILE_H__

#include "complex.h"
#include "constants.h"


class Tile {
private:
    complex _origin;
    complex _stride;
    char* _data;

public:
    Tile(complex origin, complex stride);

    ~Tile();

    complex getOrigin() { return _origin; }

    complex getStride() { return _stride; }

    char* getData() { return _data; }

    int getPoint(int x, int y) { return _data[x + y * Constants::TILE_WIDTH]; }

    void setPoint(int x, int y, int value) { _data[x + y * Constants::TILE_WIDTH] = value; }
};


#endif
