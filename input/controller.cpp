#include "controller.h"
#include <SDL2/SDL.h>

namespace input {

input::Model Controller::getInput() {
    input::Model model;

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    const double shift_speed = 1.0;
    if (keys[SDL_SCANCODE_RIGHT])   model.dx += shift_speed;
    if (keys[SDL_SCANCODE_LEFT])    model.dx -= shift_speed;
    if (keys[SDL_SCANCODE_UP])      model.dy += shift_speed;
    if (keys[SDL_SCANCODE_DOWN])    model.dy -= shift_speed;

    const double zoom_speed = 1.0;
    if (keys[SDL_SCANCODE_W])       model.dz += zoom_speed;
    if (keys[SDL_SCANCODE_S])       model.dz -= zoom_speed;

    return model;
}

}