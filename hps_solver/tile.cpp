#include "tile.h"


Tile::Tile(complex origin, complex stride) : _origin(origin), _stride(stride) {
    _data = new char [Constants::TILE_WIDTH * Constants::TILE_HEIGHT];
}


Tile::~Tile() {
    delete _data;
}