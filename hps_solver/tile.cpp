#include "tile.h"


Tile::Tile(int x_res, int y_res, complex origin, complex stride) : 
        _x_res(x_res), _y_res(y_res), _origin(origin), _stride(stride) {
    _data = new int [x_res * y_res];
}


Tile::~Tile() {
    delete _data;
}