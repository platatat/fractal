#ifndef __FPGA_SOLVER_H__
#define __FPGA_SOLVER_H__


#include "solver.h"


class FPGASolver : public Solver {
private:
    // Addresses on the bus
    volatile int16_t* out_pixel_ptr;
    volatile uint32_t* fifo_ptr;
    volatile uint32_t* fifo_control_ptr;
    volatile int16_t* out_last_pixel;

public:
    FPGASolver();

    //std::vector<uint16_t> solveTile(std::shared_ptr<TileHeader> tile, uint16_t iterations) override;

};


#endif
