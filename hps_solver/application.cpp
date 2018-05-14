#include "application.h"
#include "sdl_input_controller.h"
#include "viewport.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std::chrono;


Application::Application(std::vector<std::tuple<std::string, int>> ip_addrs) :
    _tile_manager(ip_addrs, Constants::CACHE_SIZE),
    _origin(0, 0)
{
    //moveTo(-0.5, 0, 2.5);

    moveTo(
        mpf_class("-1.769383179195515018213847286085473782905747263654751437465528216527888191264756458836163446389529667"),
        mpf_class("0.0042368479187367722149265071713679970766826709174037572794594356501123440008055451573024309950236365"),
        20
    );

    _running = false;
    control_rate = 1.0;
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

    double timestep = 0;

    while (_running) {
        auto frame_start = high_resolution_clock::now();

        setDrawColor(_clear_color);
        SDL_RenderClear(_sdl_renderer);

        _origin.real.set_prec(_zoom + 64);
        _origin.imag.set_prec(_zoom + 64);

        handleEvents();
        handleInput(timestep);
        drawFrame();
        drawHUD();

        SDL_RenderPresent(_sdl_renderer);

        auto frame_end = high_resolution_clock::now();
        double frame_time = duration_cast<microseconds>(frame_end - frame_start).count() / 1000000.0;
        double target_time = 1.0 / Constants::TARGET_FPS;
        _fps = (0.95 * _fps) + (0.05 * 1.0 / frame_time);

        SDL_Delay((int) (std::max(0.0, target_time - frame_time) * 1000));

        auto loop_end = high_resolution_clock::now();
        timestep = duration_cast<microseconds>(loop_end - frame_start).count() / 1000000.0;
    }

    SDL_DestroyWindow(_window);
    SDL_Quit();
}


void Application::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                _running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        _running = false;
                        break;

                    case SDLK_SPACE:
                        control_rate = 2.5;
                        break;

                    case SDLK_e:
                        if (_tile_manager.getIterations() < Constants::MAX_ITERS) {
                            _tile_manager.setIterations(_tile_manager.getIterations() * 1.25);
                        }
                        break;

                    case SDLK_q:
                        if (_tile_manager.getIterations() > Constants::MIN_ITERS) {
                            _tile_manager.setIterations(_tile_manager.getIterations() / 1.25);
                        }
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        control_rate = 1.0;
                        break;
                }
                break;
        }
        if (event.type == SDL_QUIT) {
            _running = false;
        }
    }
}


void Application::handleInput(double timestep) {
    SdlInputController inputController;
    auto input = inputController.getInput();

    double rate = control_rate * timestep * 60;

    _origin.real = _origin.real + (input.dx * 0.08 * rate * pow(2, -_zoom));
    _origin.imag = _origin.imag - (input.dy * 0.08 * rate * pow(2, -_zoom));

    // Translate origin to screen center before zooming.
    _origin.real = _origin.real + Viewport::screenWidth(_zoom) * 0.5;
    _origin.imag = _origin.imag + Viewport::screenHeight(_zoom) * 0.5;

    _zoom += input.dz * 0.03 * rate;

    // Translate origin back to top left.
    _origin.real = _origin.real - Viewport::screenWidth(_zoom) * 0.5;
    _origin.imag = _origin.imag - Viewport::screenHeight(_zoom) * 0.5;
}


void Application::moveTo(mpf_class x, mpf_class y, double z) {
    _zoom = z;
    _origin.real = x - Viewport::screenWidth(_zoom) * 0.5;
    _origin.imag = y - Viewport::screenHeight(_zoom) * 0.5;
}


void Application::setDrawColor(const SDL_Color& color) {
    SDL_SetRenderDrawColor(_sdl_renderer, color.r, color.g, color.b, color.a);
}


void Application::drawFrame() {
    SDL_SetRenderDrawBlendMode(_sdl_renderer, SDL_BLENDMODE_ADD);

    _tile_manager.clearRequests();

    for (int level = -5; level <= -1; level++) {
        Viewport viewport(_origin, _zoom, level);
        std::set<std::shared_ptr<Tile>> tiles = _tile_manager.loadViewport(viewport);
        _renderer.render(tiles, viewport, _sdl_renderer);
    } 
}


void Application::drawHUD() {
    SDL_SetRenderDrawBlendMode(_sdl_renderer, SDL_BLENDMODE_NONE);

    std::ostringstream ss;
    ss << "fps: " << (int) _fps << " | ";
    ss << "zoom: " << (int) _zoom << " | ";
    ss << "iter: " << _tile_manager.getIterations();
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
