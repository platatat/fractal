#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"


class CPUSolver : public Solver {
private:
    int16_t solvePixel(complex c, int16_t iterations);

protected:
    void solveTile(std::shared_ptr<TileHeader> header, Solver::data& data, int16_t iterations) override;

public:
    CPUSolver();
};


#endif
