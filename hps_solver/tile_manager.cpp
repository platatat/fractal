#include "tile_manager.h"
#include "tile_solver.h"
#include <math.h>
#include <iostream>


Tile* TileManager::loadTile(int x, int y, int z) {
    double tile_size = pow(2, -z);

    complex origin = {x * tile_size, y * tile_size};
    complex size = {tile_size, tile_size};

    Tile* tile = new Tile(origin, size);
    TileSolver::solveTile(tile, Constants::ITERATIONS);

    return tile;
}


void TileManager::loadViewport(complex origin, complex size, int z, std::vector<Tile*>& tiles) {
    double tile_length = pow(2, -z);
    int viewport_width = (int) size.real.toDouble() / tile_length + 1;
    int viewport_height = (int) size.imag.toDouble() / tile_length + 1;

    for (int y = 0; y < viewport_height; y++) {
        for (int x = 0; x < viewport_width; x++) {
            complex tile_origin = {origin.real + x * tile_length, origin.imag + y * tile_length};
            complex tile_size = {tile_length, tile_length};
            Tile* tile = new Tile(tile_origin, tile_size);
            TileSolver::solveTile(tile, Constants::ITERATIONS);
            tiles.push_back(tile);
        }
    }
}