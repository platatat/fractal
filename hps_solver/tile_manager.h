#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__

#include "constants.h"


class TileManager {
private:

public:
    // TileManager(int cache_size);

    char* loadTile(int x, int y, int z);
};


#endif