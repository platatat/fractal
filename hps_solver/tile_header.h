#ifndef __TILE_HEADER_H__
#define __TILE_HEADER_H__

#include <gmpxx.h>


struct TileHeader {
    mpz_class x;
    mpz_class y;
    long z;

    bool operator==(const TileHeader& other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
};


#endif
