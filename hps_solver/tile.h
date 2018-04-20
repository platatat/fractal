#ifndef __TILE_H__
#define __TILE_H__

#include "complex.h"
#include "constants.h"


class Tile {
private:
    complex _origin;
    complex _size;
    char* _data;

public:
    Tile(complex origin, complex size);

    ~Tile();

    complex getOrigin() { return _origin; }

    complex getSize() { return _size; }

    char* getData() { return _data; }

    int getPoint(int x, int y) { return _data[x + y * Constants::TILE_WIDTH]; }

    void setPoint(int x, int y, int value) { _data[x + y * Constants::TILE_WIDTH] = value; }
};


#endif
