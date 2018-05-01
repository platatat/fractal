#include "tile.h"


Tile::Tile(TileHeader header, unsigned char* data) : _header(header), _data(data) {}


Tile::~Tile() {}