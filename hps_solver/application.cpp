#include "application.h"
#include "sdl_input_controller.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std::chrono;


Application::Application(std::string ip_addr) :
    _origin({0.0, 0.0}),
    _zoom(2),
    _tile_manager(ip_addr, Constants::CACHE_SIZE),
    _renderer()
{
    mpf_class real("0.3750001200618655");
    mpf_class imag("-0.2166393884377127");

    double screen_width = pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
    double screen_height = pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;

    _origin = {real - screen_width * 0.5, imag - screen_width * 0.5};
    std::cout << real.get_d() << ", " << imag.get_d() << std::endl;

    _running = false;
}


void Application::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
    }

    if (TTF_Init() < 0) {
        std::cout << "TTF_Init error: " << TTF_GetError() << std::endl;
    }

    _window = SDL_CreateWindow(
        Constants::WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        Constants::SCREEN_WIDTH,
        Constants::SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (_window == nullptr) {
        std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
    }

    _window_surface = SDL_GetWindowSurface(_window);

    if (_window_surface == nullptr) {
        std::cout << "SDL_GetWindowSurface error: " << SDL_GetError() << std::endl;
    }

    _sdl_renderer = SDL_CreateRenderer(_window, -1, 0);

    if (_sdl_renderer == nullptr) {
        std::cout << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
    }

    _font_regular   = TTF_OpenFont("../data/fonts/open_sans/regular.ttf", 16);
    _font_bold      = TTF_OpenFont("../data/fonts/open_sans/bold.ttf", 16);

    if (_font_regular == nullptr || _font_bold == nullptr) {
        std::cout << "SDL_OpenFont error: " << SDL_GetError() << std::endl;
    }

    _clear_color    = { 39,  40,  34, 255};
    _color_white    = {255, 255, 255, 255};
    _color_grey     = {100, 100, 100, 255};
}


void Application::run() {
    _running = true;

    while (_running) {
        auto frame_start = high_resolution_clock::now();

        setDrawColor(_clear_color);
        SDL_RenderClear(_sdl_renderer);

        handleEvents();
        handleInput();
        drawFrame();
        drawHUD();

        SDL_RenderPresent(_sdl_renderer);

        auto frame_end = high_resolution_clock::now();
        double frame_time = duration_cast<microseconds>(frame_end - frame_start).count() / 1000000.0;
        double target_time = 1.0 / Constants::TARGET_FPS;
        _fps = (0.95 * _fps) + (0.05 * 1.0 / frame_time);

        SDL_Delay((int) (std::max(0.0, target_time - frame_time) * 1000));
    }

    SDL_DestroyWindow(_window);
    SDL_Quit();
}


void Application::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            _running = false;
        }
    }
}


void Application::handleInput() {
    SdlInputController inputController;
    auto input = inputController.getInput();

    _origin.real = _origin.real + (input.dx * 0.1 * pow(2, -_zoom));
    _origin.imag = _origin.imag - (input.dy * 0.1 * pow(2, -_zoom));

    // Translate origin to screen center before zooming.
    _origin.real = _origin.real + getScreenWidth() * 0.5;
    _origin.imag = _origin.imag + getScreenHeight() * 0.5;

    _zoom += input.dz * 0.04;

    // Translate origin back to top left.
    _origin.real = _origin.real - getScreenWidth() * 0.5;
    _origin.imag = _origin.imag - getScreenHeight() * 0.5;
}


void Application::setDrawColor(const SDL_Color& color) {
    SDL_SetRenderDrawColor(_sdl_renderer, color.r, color.g, color.b, color.a);
}


double Application::getScreenWidth() {
    return pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
}


double Application::getScreenHeight() {
    return pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;
}


void Application::drawFrame() {
    SDL_SetRenderDrawBlendMode(_sdl_renderer, SDL_BLENDMODE_ADD);

    int mipmap_level = (int) std::ceil(_zoom);
    double fractional_scale = std::pow(2.0, -(mipmap_level - _zoom));

    std::vector<std::shared_ptr<Tile>> tiles;
    complex screen_size = {getScreenWidth(), getScreenHeight()};
    auto viewport_info = _tile_manager.loadViewport(_origin, screen_size, mipmap_level, tiles);

    _renderer.render(viewport_info, tiles, fractional_scale, _sdl_renderer);
}


void Application::drawHUD() {
    SDL_SetRenderDrawBlendMode(_sdl_renderer, SDL_BLENDMODE_NONE);

    std::ostringstream ss;
    ss << "fps: " << (int) _fps << " | ";
    ss << "zoom: " << (int) _zoom;
    std::string hud_string = ss.str();

    SDL_Surface* message_surface = TTF_RenderText_Solid(_font_regular, hud_string.c_str(), _color_white);
    SDL_Texture* message_texture = SDL_CreateTextureFromSurface(_sdl_renderer, message_surface);
    SDL_Rect message_rect = {5, 5, message_surface->w, message_surface->h};

    setDrawColor(_color_grey);
    SDL_RenderFillRect(_sdl_renderer, &message_rect);
    SDL_RenderCopy(_sdl_renderer, message_texture, NULL, &message_rect);

    SDL_FreeSurface(message_surface);
    SDL_DestroyTexture(message_texture);
}
