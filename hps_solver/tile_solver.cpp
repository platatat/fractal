#include "tile_solver.h"
#include "constants.h"


void TileSolver::solveTile(std::shared_ptr<Tile> tile, int iterations) {
    complex origin = tile->getOrigin();
    complex size = {tile->getSize(), tile->getSize()};
    complex stride = {size.real.toDouble() / Constants::TILE_WIDTH, 
                      size.imag.toDouble() / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real.toDouble() * x_index + origin.real.toDouble(),
                         stride.imag.toDouble() * y_index + origin.imag.toDouble()};
            int solution = solvePixel(c, iterations);
            tile->setPoint(x_index, y_index, solution);
        }
    }
}


int TileSolver::solvePixel(complex c, int iterations) {
    complex z = c;

    for (int i = 1; i < iterations - 1; i++) {
        BFloat z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        BFloat z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        if (z.real * z.real + z.imag * z.imag > 4) {
            return i;
        }
    }

    return iterations - 1;
}