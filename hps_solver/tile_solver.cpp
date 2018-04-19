#include "tile_solver.h"


void TileSolver::solveTile(Tile& tile, int iterations) {
    for (int y_index = 0; y_index < tile.getYRes(); y_index++) {
        for (int x_index = 0; x_index < tile.getXRes(); x_index++) {
            complex c = {.real = tile.getStride().real * x_index + tile.getOrigin().real, 
                         .imag = tile.getStride().imag * y_index + tile.getOrigin().imag};
            int solution = solvePixel(c, iterations);
            tile.setPoint(x_index, y_index, solution);
        }
    }
}


int TileSolver::solvePixel(complex c, int iterations) {
    complex z = c;

    for (int i = 0; i < iterations; i++) {
        BigNum z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        BigNum z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        if (z.real * z.real + z.imag * z.imag > 4) {
            // Add 2 because i is zero-indexed and we skip an iteration with z0 = c.
            return i + 2;
        }
    }

    return -1;
}