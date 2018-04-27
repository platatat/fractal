#include "tile_solver.h"
#include "constants.h"


void TileSolver::solveTile(Tile* tile, int iterations) {
    complex origin = tile->getOrigin();
    complex size = {tile->getSize(), tile->getSize()};
    complex stride = {size.real.toDouble() / Constants::TILE_WIDTH, 
                      size.imag.toDouble() / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real.toDouble() * x_index + origin.real.toDouble(),
                         stride.imag.toDouble() * y_index + origin.imag.toDouble()};
            int solution = solvePixel(c, iterations);
            tile->setPoint(x_index, y_index, solution * 7);
        }
    }
}


int TileSolver::solvePixel(complex c, int iterations) {
    complex z = c;

    for (int i = 0; i < iterations - 1; i++) {
        BigNum z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        BigNum z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        if (z.real * z.real + z.imag * z.imag > 4) {
            // Add 1 because we skip an iteration with z0 = c.
            return i + 1;
        }
    }

    return iterations;
}