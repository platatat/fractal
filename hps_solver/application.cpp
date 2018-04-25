#include "application.h"

#include "sdl_input_controller.h"

Application::Application() :
    _origin({0, 0}),
    _zoom(2),
    _tile_manager(512),
    _renderer()
{
}

const unsigned char* Application::display() {
    // Process input
    SdlInputController inputController;
    auto input = inputController.getInput();

    _origin.real = _origin.real + (input.dx * 0.02);
    _origin.imag = _origin.imag - (input.dy * 0.02);

    _zoom += input.dz * 0.01;

    // Generate the tiles
    std::vector<Tile*> tiles;

    double screen_width = pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
    double screen_height = pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;
    complex screen_size = {screen_width, screen_height};

    int mipmap_level = (int) std::ceil(_zoom);
    double fractional_scale = std::pow(2., -(mipmap_level - _zoom));

    auto viewportInfo = _tile_manager.loadViewport(_origin, screen_size, mipmap_level, tiles);

    // Render the tiles
    _renderer.render(viewportInfo, tiles, fractional_scale);

    return _renderer.getScreenBuffer();
}

int Application::getDisplayStride() {
    return _renderer.getScreenBufferStride();
}
