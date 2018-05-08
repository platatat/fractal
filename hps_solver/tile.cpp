#include "tile.h"


Tile::Tile(TileHeader header, unsigned char* data, bool is_placeholder) : 
        _header(header), 
        _data(data), 
        _is_placeholder(is_placeholder) {

}


Tile::~Tile() {}


complex Tile::getOrigin() const {
    mpf_class f_header_x(_header.x);
    mpf_class f_header_y(_header.y);
    return {f_header_x * getSize(), f_header_y * getSize()};
}