#include "tile_manager.h"
#include "tile.h"
#include "tile_solver.h"
#include "complex.h"
#include <math.h>
#include <iostream>


char* TileManager::loadTile(int x, int y, int z) {
    double ms_tile_size = pow(2, -z);

    double stride_x = ms_tile_size / Constants::TILE_WIDTH;
    double stride_y = ms_tile_size / Constants::TILE_HEIGHT;

    complex origin = {x * ms_tile_size, y * ms_tile_size};
    complex stride = {stride_x, stride_y};

    Tile* tile = new Tile(origin, stride);
    TileSolver::solveTile(*tile, Constants::ITERATIONS);

    return tile->getData();
}