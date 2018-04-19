#include <iostream>
#include "tile_solver.h"
#include "complex.h"

using namespace std;


bool test_tile_solver() {
    int x_res = 20;
    int y_res = 20;
    complex origin = {.real = -1.0, .imag = -1.0};
    complex stride = {.real = 0.1, .imag = 0.1};

    Tile tile = Tile(x_res, y_res, origin, stride);
    TileSolver::solveTile(tile, 100);

    if (tile.getPoint(1 , 1 ) != 3)   return false;
    if (tile.getPoint(10, 10) != -1)  return false;
    if (tile.getPoint(13, 3 ) != 6)   return false;
    if (tile.getPoint(2 , 8 ) != 15)  return false;
    
    cout << "success\n";
    return true;
}


int main() {
    test_tile_solver();
}