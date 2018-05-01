#ifndef __TILE_HEADER_H__
#define __TILE_HEADER_H__

#include "big_int.h"


struct TileHeader {
    BInt x;
    BInt y;
    long z;

    bool operator==(const TileHeader& other) const {
            return (x == other.x) && (y == other.y) && (z == other.z);
        }
};


#endif
