#include "application.h"


Application::Application() {
}

const unsigned char* Application::display() {
    _renderer.render();
    return _renderer.getScreenBuffer();
}

int Application::getDisplayStride() {
    return _renderer.getScreenBufferStride();
}
