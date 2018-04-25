#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <vector>

#include "tile_manager.h"

class Renderer {
private:
    class Pimpl;
    std::unique_ptr<Pimpl> pimpl;

public:
    Renderer();
    ~Renderer();

    void render(const TileManager::ViewportInfo& viewportInfo,
                const std::vector<Tile*>& tiles);

    const unsigned char* getScreenBuffer();
    int getScreenBufferStride();
};


#endif
