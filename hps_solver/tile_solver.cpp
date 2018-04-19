#include "tile_solver.h"
#include <iostream>


void TileSolver::solveTile(complex origin, complex stride, int x_res, int y_res, 
                           int iterations, int* output) {
    for (int y_index = 0; y_index < y_res; y_index++) {
        for (int x_index = 0; x_index < x_res; x_index++) {
            complex c = {.real = x_index * stride.real + origin.real, 
                         .imag = y_index * stride.imag + origin.imag};
            int solution = solvePixel(c, iterations);
            int index = y_index * x_res + x_index;
            output[index] = solution;
        }
    }
}


int TileSolver::solvePixel(complex c, int iterations) {
    complex z = c;

    for (int i = 0; i < iterations; i++) {
        double z_real_new = (z.real * z.real) - (z.imag * z.imag) + c.real;
        double z_imag_new = (2 * z.imag * z.real) + c.imag;
        z = {.real = z_real_new, .imag = z_imag_new};

        // std::cout << z.real << ", " << z.imag << "\n";

        if (z.real * z.real + z.imag * z.imag > 4) {
            // Add 2 because i is zero-indexed and we skip an iteration with z0 = c.
            return i + 2;
        }
    }

    return -1;
}