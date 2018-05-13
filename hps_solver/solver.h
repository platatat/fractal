#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "tile_header.h"

#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>


class Solver {
public:
    typedef std::unique_ptr<volatile int16_t[], std::function<void(volatile int16_t*)>> data;

    virtual ~Solver() = 0;

    void sumbit(std::shared_ptr<TileHeader> tile, int16_t iterations);
    Solver::data retrieve(std::shared_ptr<TileHeader> tile);

protected:
    void freeListAppend(volatile int16_t* data);
    virtual void solveTile(std::shared_ptr<TileHeader> header, Solver::data& data, int16_t iterations) = 0;

private:
    std::mutex mutex;
    std::condition_variable has_space;
    std::deque<Solver::data> free_list;

    std::map<std::shared_ptr<TileHeader>, Solver::data> inflight;

};

inline Solver::~Solver() { }

#endif
