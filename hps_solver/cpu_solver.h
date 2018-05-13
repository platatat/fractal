#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"


class CPUSolver : public Solver {
private:
    uint16_t solvePixel(complex c, uint16_t iterations);

protected:
    void solveTile(std::shared_ptr<TileHeader> header, Solver::data& data, uint16_t iterations) override;

public:
    CPUSolver();
};


#endif
