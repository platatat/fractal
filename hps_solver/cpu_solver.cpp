#include "cpu_solver.h"

#include "constants.h"

#include <iostream>


#define NUM_SLOTS 3


CPUSolver::CPUSolver() {
    for (int i = 0; i < NUM_SLOTS; i++) {
        freeListAppend(new uint16_t[Constants::TILE_WIDTH * Constants::TILE_HEIGHT]);
    }
}


void CPUSolver::freeListAppend(volatile uint16_t* data) {
    std::unique_lock<std::mutex> lock(mutex);
    free_list.emplace_back(data, [this] (volatile uint16_t* data) { deleteData(data); });
    has_space.notify_all();
}


void CPUSolver::deleteData(volatile uint16_t* data) {
    freeListAppend(data);
}


void CPUSolver::sumbit(std::shared_ptr<TileHeader> tile, uint16_t iterations) {
    std::unique_lock<std::mutex> lock(mutex);
    while (free_list.empty()) has_space.wait(lock);

    Solver::data data = std::move(free_list.front());
    free_list.pop_front();
    data[Constants::TILE_WIDTH * Constants::TILE_HEIGHT - 1] = -2;
    inflight[tile] = std::move(data);

    solveTile(tile, iterations);
}


Solver::data CPUSolver::retrieve(std::shared_ptr<TileHeader> tile) {
    std::unique_lock<std::mutex> lock(mutex);

    Solver::data& data = inflight[tile];
    if (data == nullptr) return nullptr;
    if (data[Constants::TILE_WIDTH * Constants::TILE_HEIGHT - 1] == -2) return nullptr;
    Solver::data ret = std::move(inflight[tile]);
    inflight.erase(tile);
    return ret;
}


void CPUSolver::solveTile(std::shared_ptr<TileHeader> header, uint16_t iterations) {
    Solver::data& tile_data = inflight[header];

    complex origin = header->getOrigin();
    complex size = {header->getSize(), header->getSize()};
    complex stride = {size.real / Constants::TILE_WIDTH, size.imag / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real * x_index + origin.real, stride.imag * y_index + origin.imag};
            uint16_t solution = solvePixel(c, iterations);
            tile_data[y_index * Constants::TILE_WIDTH + x_index] = solution;
        }
    }
}


uint16_t CPUSolver::solvePixel(complex c, uint16_t iterations) {
    complex z = c;

    complex cycle_z = c;

    for (uint16_t i = 1; i < iterations - 1; i++) {
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
