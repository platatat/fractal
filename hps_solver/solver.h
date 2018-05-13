#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "tile_header.h"

#include <memory>
#include <vector>

class Solver {
public:
    virtual ~Solver() = 0;

    virtual std::vector<uint16_t> solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations) = 0;

};

inline Solver::~Solver() { }

#endif
