#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "complex.h"
#include "tile.h"
#include <vector>


class TileManager {
private:
    Tile* loadTile(int x, int y, int z);

public:
    void loadViewport(complex origin, complex size, int z, std::vector<Tile*>& tiles);
};


#endif
