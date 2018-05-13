#include "cpu_solver.h"
#include "constants.h"
#include <iostream>


std::vector<uint16_t> CPUSolver::solveTile(std::shared_ptr<TileHeader> header, uint16_t iterations) {
    std::vector<uint16_t> tile_data;

    complex origin = header->getOrigin();
    complex size = {header->getSize(), header->getSize()};
    complex stride = {size.real / Constants::TILE_WIDTH, size.imag / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real * x_index + origin.real, stride.imag * y_index + origin.imag};
            uint16_t solution = solvePixel(c, iterations);
            tile_data.push_back(solution);
        }
    }

    return tile_data;
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
