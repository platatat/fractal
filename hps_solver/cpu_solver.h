#ifndef __CPU_SOLVER_H__
#define __CPU_SOLVER_H__


#include "solver.h"

#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>


class CPUSolver : public Solver {
private:
    std::mutex mutex;
    std::condition_variable has_space;
    std::deque<Solver::data> free_list;

    std::map<std::shared_ptr<TileHeader>, Solver::data> inflight;

    void freeListAppend(volatile uint16_t* data);
    void deleteData(volatile uint16_t* data);

    void solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations);
    uint16_t solvePixel(complex c, uint16_t iterations);

public:
    CPUSolver();

    void sumbit(std::shared_ptr<TileHeader> tile, uint16_t iterations) override;
    Solver::data retrieve(std::shared_ptr<TileHeader> tile) override;
};


#endif
