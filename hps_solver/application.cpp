#include "application.h"
#include "sdl_input_controller.h"
#include "big_float.h"
#include <cmath>

Application::Application() :
    _origin({0.0, 0.0}),
    _zoom(2),
    _tile_manager(20000),
    _renderer()
{
}

const unsigned char* Application::display() {
    // Process input
    SdlInputController inputController;
    auto input = inputController.getInput();

    _origin.real = _origin.real + (input.dx * 0.1 * pow(2, -_zoom));
    _origin.imag = _origin.imag - (input.dy * 0.1 * pow(2, -_zoom));

    double prezoom_screen_width = pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
    double prezoom_screen_height = pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;

    _origin.real = _origin.real + prezoom_screen_width * 0.5;
    _origin.imag = _origin.imag + prezoom_screen_height * 0.5;

    _zoom += input.dz * 0.04;

    double screen_width = pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
    double screen_height = pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;

    _origin.real = _origin.real - screen_width * 0.5;
    _origin.imag = _origin.imag - screen_height * 0.5;

    int mipmap_level = (int) std::ceil(_zoom);
    double fractional_scale = std::pow(2., -(mipmap_level - _zoom));

    // Generate the tiles
    std::vector<std::shared_ptr<Tile>> tiles;
    complex screen_size = {screen_width, screen_height};
    auto viewportInfo = _tile_manager.loadViewport(_origin, screen_size, mipmap_level, tiles);

    // Render the tiles
    _renderer.render(viewportInfo, tiles, fractional_scale);

    return _renderer.getScreenBuffer();
}

int Application::getDisplayStride() {
    return _renderer.getScreenBufferStride();
}
