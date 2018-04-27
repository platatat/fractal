#ifndef __TILE_H__
#define __TILE_H__

#include "complex.h"
#include "constants.h"
#include "tile_header.h"
#include <math.h>


class Tile {
private:
    TileHeader _header;
    unsigned char* _data;

public:
    Tile(TileHeader header);

    ~Tile();

    complex getOrigin() const { return {_header.x * getSize(), _header.y * getSize()}; }

    double getSize() const { return pow(2, -_header.z); }

    TileHeader getHeader() const { return _header; }

    unsigned char* getData() const { return _data; }

    int getPoint(int x, int y) const { return _data[x + y * Constants::TILE_WIDTH]; }

    void setPoint(int x, int y, int value) { _data[x + y * Constants::TILE_WIDTH] = value; }
};


#endif
