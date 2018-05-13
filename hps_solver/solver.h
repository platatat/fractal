#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "tile_header.h"

#include <functional>
#include <memory>
#include <vector>

class Solver {
public:
    typedef std::unique_ptr<volatile uint16_t[], std::function<void(volatile uint16_t*)>> data;

    virtual ~Solver() = 0;

    virtual void sumbit(std::shared_ptr<TileHeader> tile, uint16_t iterations) = 0;
    virtual Solver::data retrieve(std::shared_ptr<TileHeader> tile) = 0;

};

inline Solver::~Solver() { }

#endif
