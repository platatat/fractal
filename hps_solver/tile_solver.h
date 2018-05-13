#ifndef __TILE_SOLVER_H__
#define __TILE_SOLVER_H__


#include "complex.h"
#include "tile.h"
#include <memory>


class TileSolver {
public:
    std::vector<uint16_t> solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations);

private:
    uint16_t solvePixel(complex c, uint16_t iterations);
};


#endif
