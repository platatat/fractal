#include "tile.h"


Tile::Tile(complex origin, complex size) : _origin(origin), _size(size) {
    _data = new char [Constants::TILE_WIDTH * Constants::TILE_HEIGHT];
}


Tile::~Tile() {
    delete _data;
}