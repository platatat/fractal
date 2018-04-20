#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>

#include "complex.h"
#include "tile_manager.h"


class Renderer {
private:
    complex _origin;
    double _zoom;

    TileManager _tile_manager;

    class Pimpl;
    std::unique_ptr<Pimpl> pimpl;

public:
    Renderer();
    ~Renderer();

    void render();

    const unsigned char* getScreenBuffer();
    int getScreenBufferStride();
};


#endif
