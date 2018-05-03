#include "tile_solver.h"
#include "constants.h"
#include <iostream>


void TileSolver::solveTile(std::shared_ptr<Tile> tile, int iterations) {
    complex origin = tile->getOrigin();
    complex size = {tile->getSize(), tile->getSize()};
    complex stride = {size.real / Constants::TILE_WIDTH, 
                      size.imag / Constants::TILE_HEIGHT};

    for (int y_index = 0; y_index < Constants::TILE_HEIGHT; y_index++) {
        for (int x_index = 0; x_index < Constants::TILE_WIDTH; x_index++) {
            complex c = {stride.real * x_index + origin.real,
                         stride.imag * y_index + origin.imag};
            int solution = solvePixel(c, iterations);            
            tile->setPoint(x_index, y_index, solution);
        }
    }
}


int TileSolver::solvePixel(complex c, int iterations) {
    complex z = c;

    // std::cout << z.real.get_d() << ", " << z.imag.get_d() << "\n";

    for (int i = 1; i < iterations - 1; i++) {
        mpf_class z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        mpf_class z_imag_new = (z.imag * z.real * 2) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        // std::cout << z.real.get_d() << ", " << z.imag.get_d() << "\n";

        if (z.real * z.real + z.imag * z.imag > 4) {
            return i;
        }
    }

    return iterations - 1;
}