#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"

#include <thread>


class CPUSolver : public Solver {
private:
    typedef std::tuple<std::shared_ptr<TileHeader>, int16_t> job;

    std::condition_variable has_jobs;
    std::deque<CPUSolver::job> jobs;
    std::vector<std::thread> solvers;
    int16_t* sram_base_ptr;

    static void solverTask(CPUSolver* solver);

    void solveTile(std::shared_ptr<TileHeader> header, int16_t iterations);
    int16_t solvePixel(complex c, int16_t iterations);

protected:
    void queueTile(std::shared_ptr<TileHeader> header, int16_t iterations);

public:
    CPUSolver();
};


#endif
