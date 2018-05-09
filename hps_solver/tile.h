#ifndef __TILE_H__
#define __TILE_H__

#include "complex.h"
#include "constants.h"
#include "tile_header.h"
#include <math.h>


class Tile {
private:
    std::shared_ptr<TileHeader> _header;
    unsigned char* const _data;
    bool _is_placeholder;

public:
    Tile(std::shared_ptr<TileHeader> header, unsigned char* data, bool is_placeholder = false);

    ~Tile();

    complex getOrigin() const;

    double getSize() const { return pow(2, -_header->z); }

    std::shared_ptr<TileHeader> getHeader() const { return _header; }

    unsigned char* const getData() const { return _data; }

    int getPoint(int x, int y) const { return _data[x + y * Constants::TILE_WIDTH]; }

    void setPoint(int x, int y, int value) { _data[x + y * Constants::TILE_WIDTH] = value; }

    bool const isPlaceholder() const { return _is_placeholder; }
};


#endif
