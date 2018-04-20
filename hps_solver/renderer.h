#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "complex.h"
#include "tile_manager.h"


class Renderer {
private:
    complex _origin;
    double _zoom;

    char* _screen_buffer;

    TileManager _tile_manager;

public:
    Renderer();

    void render();

    char* getScreenBuffer() { return _screen_buffer; }
};


#endif