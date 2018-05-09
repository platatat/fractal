#ifndef __TILE_SOLVER_H__
#define __TILE_SOLVER_H__


#include "complex.h"
#include "tile.h"
#include <memory>


class TileSolver {
public:
    static std::vector<uint8_t> solveTile(std::shared_ptr<TileHeader> tile, uint8_t iterations);

    static uint8_t solvePixel(complex c, uint8_t iterations);
};


#endif
