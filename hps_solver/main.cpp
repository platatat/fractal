#include <iostream>
#include "tile_solver.h"
#include "complex.h"

using namespace std;


bool test_tile_solver() {
    int x_res = 20;
    int y_res = 20;
    complex origin = {.real = -1.0, .imag = -1.0};
    complex stride = {.real = 0.1, .imag = 0.1};

    int* output = new int [x_res * y_res];
    TileSolver::solveTile(origin, stride, x_res, y_res, 100, output);

    if (output[1  + 1  * x_res] != 3)   return false;
    if (output[10 + 10 * x_res] != -1)  return false;
    if (output[13 + 3  * x_res] != 6)   return false;
    if (output[2  + 8  * x_res] != 15)  return false;
    
    cout << "success\n";
    return true;
}


int main() {
    test_tile_solver();
}