#include "solver.h"
#include "constants.h"


std::vector<uint16_t> Solver::solveTile(std::shared_ptr<TileHeader> header) {
    std::vector<uint16_t> data;

    complex origin = header->getOrigin();
    complex size(header->getSize(), header->getSize(), header->z + 64);
    complex stride(size.real / Constants::TILE_WIDTH, size.imag / Constants::TILE_HEIGHT);

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c(stride.real * x_index + origin.real, stride.imag * y_index + origin.imag);
            uint16_t solution = solvePixel(c, header->i);
            data.push_back(solution);
        }
    }

    return data;
}


uint16_t Solver::solvePixel(complex c, int16_t iterations) {
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
