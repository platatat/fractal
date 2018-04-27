#include "tile.h"


Tile::Tile(TileHeader header) : _header(header) {
    _data = new unsigned char [Constants::TILE_WIDTH * Constants::TILE_HEIGHT];
}


Tile::~Tile() {
    delete _data;
}