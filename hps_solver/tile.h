#ifndef __TILE_H__
#define __TILE_H__

#include "complex.h"


class Tile {
private:
    int _x_res;
    int _y_res;
    complex _origin;
    complex _stride;
    char* _data;

public:
    Tile(int x_res, int y_res, complex origin, complex stride);

    ~Tile();

    int getXRes() { return _x_res; }

    int getYRes() { return _y_res; }

    complex getOrigin() { return _origin; }

    complex getStride() { return _stride; }

    char* getData() { return _data; }

    int getPoint(int x, int y) { return _data[x + y * _x_res]; }

    void setPoint(int x, int y, int value) { _data[x + y * _x_res] = value; }
};


#endif
