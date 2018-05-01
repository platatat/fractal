#ifndef __TILE_SOLVER_H__
#define __TILE_SOLVER_H__


#include "complex.h"
#include "tile.h"
#include <memory>


class TileSolver {
public:
    static void solveTile(std::shared_ptr<Tile> tile, int iterations);

    static int solvePixel(complex c, int iterations);
};


#endif
