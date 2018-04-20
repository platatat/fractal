#include "tile_manager.h"
#include "tile.h"
#include "tile_solver.h"
#include "constants.h"
#include "complex.h"
#include <math.h>
#include <stdio.h>
#include <iostream>


char* TileManager::loadTile(int x, int y, int z) {
    // Dimensions of a tile in mandelbrot space at this zoom level.
    double ms_tile_width = Constants::TILE_WIDTH * pow(2, -z);
    double ms_tile_height = Constants::TILE_HEIGHT * pow(2, -z);

    complex origin = {-0.75 * ms_tile_width, -0.5 * ms_tile_height};
    complex stride = {pow(2, -z), pow(2, -z)};

    Tile* tile = new Tile(Constants::TILE_WIDTH, Constants::TILE_HEIGHT, origin, stride);
    TileSolver::solveTile(*tile, Constants::ITERATIONS);

    return tile->getData();
}