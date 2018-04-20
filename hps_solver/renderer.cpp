#include "renderer.h"


Renderer::Renderer() : _origin({0, 0}), _zoom(0) {
    _screen_buffer = new char [Constants::SCREEN_WIDTH * Constants::SCREEN_HEIGHT];
}


void Renderer::render() {
    _screen_buffer = _tile_manager.loadTile(1, 0, 2);
}