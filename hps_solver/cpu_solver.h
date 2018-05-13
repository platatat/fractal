#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"


class CPUSolver : public Solver {
public:
    std::vector<uint16_t> solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations) override;

private:
    uint16_t solvePixel(complex c, uint16_t iterations);
};


#endif
