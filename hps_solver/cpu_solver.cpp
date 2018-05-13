#include "cpu_solver.h"

#include "constants.h"

#include <iostream>


#define NUM_SLOTS 4


CPUSolver::CPUSolver() {
    sram_base_ptr = new int16_t[NUM_SLOTS * Constants::TILE_WIDTH * Constants::TILE_HEIGHT];

    for (int i = 0; i < NUM_SLOTS; i++) {
        freeListAppend(sram_base_ptr + (i * Constants::TILE_WIDTH * Constants::TILE_HEIGHT));
    }

    for (int i = 0; i < NUM_SLOTS; i++) {
        solvers.emplace_back(solverTask, this);
    }
}


void CPUSolver::solverTask(CPUSolver* solver) {
    while (true) {
        std::shared_ptr<TileHeader> header;
        int16_t iterations;
        {
            std::unique_lock<std::mutex> lock(solver->mutex);
            while (solver->jobs.empty()) solver->has_jobs.wait(lock);

            CPUSolver::job job = solver->jobs.front();
            solver->jobs.pop_front();
            header = std::get<0>(job);
            iterations = std::get<1>(job);
        }
        solver->solveTile(header, iterations);
    }
}


void CPUSolver::queueTile(std::shared_ptr<TileHeader> header, int16_t iterations) {
    jobs.emplace_back(header, iterations);
    has_jobs.notify_all();
}


void CPUSolver::solveTile(std::shared_ptr<TileHeader> header, int16_t iterations) {
    Solver::data& data = inflight[header];

    complex origin = header->getOrigin();
    complex size = {header->getSize(), header->getSize()};
    complex stride = {size.real / Constants::TILE_WIDTH, size.imag / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real * x_index + origin.real, stride.imag * y_index + origin.imag};
            uint16_t solution = solvePixel(c, iterations);

            std::unique_lock<std::mutex> lock(mutex);
            data[y_index * Constants::TILE_WIDTH + x_index] = solution;
        }
    }
}


int16_t CPUSolver::solvePixel(complex c, int16_t iterations) {
    complex z = c;

    complex cycle_z = c;

    for (int16_t i = 1; i < iterations - 1; i++) {
        mpf_class z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        mpf_class z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {z_real_new, z_imag_new};

        if (z == cycle_z) {
            break;
        }

        if ((i & (~i + 1)) == i) {
            cycle_z = z;
        }

        if (z.real * z.real + z.imag * z.imag > 4) {
            return i;
        }
    }

    return iterations - 1;
}
