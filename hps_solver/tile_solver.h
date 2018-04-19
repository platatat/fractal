#ifndef __TILE_SOLVER_H__
#define __TILE_SOLVER_H__


#include "complex.h"


class TileSolver {
public:
    static void solveTile(complex origin, complex stride, int x_res, int y_res, 
                          int iterations, int* output);

    static int solvePixel(complex c, int iterations);
};


#endif
