#include "application.h"


Application::Application() {
    _renderer = Renderer();
}


char* Application::display() {
    _renderer.render();
    return _renderer.getScreenBuffer();
}