#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "tile_header.h"
#include <vector>


class Solver {
public:
    static std::vector<uint16_t> solveTile(std::shared_ptr<TileHeader> header);

private:
    static uint16_t solvePixel(complex c, int16_t iterations);
};


#endif
