#include "tile_solver.h"
#include "constants.h"
#include <iostream>


std::vector<uint8_t> TileSolver::solveTile(std::shared_ptr<TileHeader> header, uint8_t iterations) {
    std::vector<uint8_t> tile_data;

    complex origin = header->getOrigin();
    complex size = {header->getSize(), header->getSize()};
    complex stride = {size.real / Constants::TILE_WIDTH, size.imag / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real * x_index + origin.real, stride.imag * y_index + origin.imag};
            uint8_t solution = solvePixel(c, iterations);            
            tile_data.push_back(solution);
        }
    }

    return tile_data;
}


uint8_t TileSolver::solvePixel(complex c, uint8_t iterations) {
    complex z = c;

    for (uint8_t i = 1; i < iterations - 1; i++) {
        mpf_class z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        mpf_class z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        if (z.real * z.real + z.imag * z.imag > 4) {
            return i;
        }
    }

    return iterations - 1;
}