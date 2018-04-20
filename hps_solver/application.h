#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "renderer.h"


class Application {
private:
    Renderer _renderer;

public:
    Application();

    char* display();
};


#endif