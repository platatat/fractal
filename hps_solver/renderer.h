#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <vector>

#include "tile_manager.h"

class Renderer {
private:
    unsigned char* _data_buffer;
    unsigned char* _screen_buffer;

    void histogramColor();

public:
    Renderer();
    ~Renderer();

    void render(const TileManager::ViewportInfo& viewportInfo,
                const std::vector<std::shared_ptr<Tile>>& tiles,
                double fractional_scale);

    const unsigned char* getScreenBuffer();
    int getScreenBufferStride();
};


#endif
