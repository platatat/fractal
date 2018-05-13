#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"

#include <map>


class CPUSolver : public Solver {
private:
    std::map<std::shared_ptr<TileHeader>, Solver::data> inflight;

    void deleteData(volatile uint16_t* data);

    void solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations);
    uint16_t solvePixel(complex c, uint16_t iterations);

public:
    void sumbit(std::shared_ptr<TileHeader> tile, uint16_t iterations) override;
    Solver::data retrieve(std::shared_ptr<TileHeader> tile) override;
};


#endif
