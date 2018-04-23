#include "renderer.h"
#include <iostream>


Renderer::Renderer() : _origin({0, 0}), _zoom(0) {
    _screen_buffer = new char [Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT];
}


void Renderer::render() {
    std::vector<Tile*> tiles;
    // _tile_manager.loadViewport(_origin, {1, 1}, 0, tiles);
    _screen_buffer = _tile_manager.requestTile({0, 0, 0})->getData();
}