#ifndef __TILE_HEADER_H__
#define __TILE_HEADER_H__


struct TileHeader {
    int x;
    int y;
    int z;

    bool operator==(const TileHeader& other) const {
            return (x == other.x) && (y == other.y) && (z == other.z);
        }
};


#endif
