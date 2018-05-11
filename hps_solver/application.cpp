#include "application.h"
#include "sdl_input_controller.h"
#include <cmath>
#include <iostream>
// #include <SDL2/SDL_ttf.h>


using namespace std::chrono;


Application::Application() :
    _origin({0.0, 0.0}),
    _zoom(2),
    _tile_manager(Constants::CACHE_SIZE),
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
}


void Application::run() {
    _running = true;

    while (_running) {
        auto frame_start = high_resolution_clock::now();

        handleEvents();
        handleInput();
        display();

        auto frame_end = high_resolution_clock::now();
        double frame_time = duration_cast<microseconds>(frame_end - frame_start).count() / 1000000.0;
        double target_time = 1.0 / Constants::TARGET_FPS;

        std::cout << (1.0 / frame_time) << std::endl;

        // std::cout << "A\n";

        // TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24); //this opens a font style and sets a size

        // SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

        // SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

        // std::cout << "B\n";

        // SDL_Texture* Message = SDL_CreateTextureFromSurface(_sdl_renderer, surfaceMessage); //now you can convert it into a texture

        // std::cout << "C\n";

        // SDL_Rect Message_rect; //create a rect
        // Message_rect.x = 0;  //controls the rect's x coordinate 
        // Message_rect.y = 0; // controls the rect's y coordinte
        // Message_rect.w = 100; // controls the width of the rect
        // Message_rect.h = 100; // controls the height of the rect

        // //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

        // //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

        // SDL_RenderCopy(_sdl_renderer, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

        // std::cout << "D\n";

        // //Don't forget too free your surface and texture

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


double Application::getScreenWidth() {
    return pow(2, -_zoom) * Constants::SCREEN_WIDTH / Constants::TILE_WIDTH;
}


double Application::getScreenHeight() {
    return pow(2, -_zoom) * Constants::SCREEN_HEIGHT / Constants::TILE_HEIGHT;
}


void Application::display() {
    int mipmap_level = (int) std::ceil(_zoom);
    double fractional_scale = std::pow(2., -(mipmap_level - _zoom));

    // Generate the tiles
    std::vector<std::shared_ptr<Tile>> tiles;
    complex screen_size = {getScreenWidth(), getScreenHeight()};

    auto viewportInfo = _tile_manager.loadViewport(_origin, screen_size, mipmap_level, tiles);

    // Render the tiles
    _renderer.render(viewportInfo, tiles, fractional_scale);

    // Draw to the screen.
    const unsigned char* buffer = _renderer.getScreenBuffer();

    SDL_Surface* buffer_surface = SDL_CreateRGBSurfaceFrom(
        (void*) buffer,
        Constants::SCREEN_WIDTH,
        Constants::SCREEN_HEIGHT,
        24, 
        _renderer.getScreenBufferStride(),
        0, 0, 0, 0
    );

    SDL_Texture* screen_texture = SDL_CreateTextureFromSurface(_sdl_renderer, buffer_surface);
    SDL_RenderCopy(_sdl_renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(_sdl_renderer);

    SDL_FreeSurface(buffer_surface);
}
