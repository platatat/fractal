#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "tile_manager.h"
#include "viewport.h"
#include <memory>
#include <set>
#include <SDL2/SDL.h>


class Renderer {
private:
    uint8_t* _colored_buffer;

    struct CachedTexture {
        SDL_Texture* texture;
        std::chrono::time_point<std::chrono::system_clock> last_hit;
    };

    SDL_Texture* createTextureForTile(std::shared_ptr<Tile> tile, SDL_Renderer* sdl_renderer);

    std::unordered_map<std::shared_ptr<TileHeader>, CachedTexture, TileHeader::Hasher, TileHeader::Comparator> _cache;

    void cacheInsert(std::shared_ptr<TileHeader> header, SDL_Texture* texture);

    bool cacheContains(std::shared_ptr<TileHeader> header);

    void cacheEvictOldest();

    void histogramColor();
    SDL_Color cyclicColor(int16_t iterations, int16_t iter_lim);

public:
    Renderer();
    ~Renderer();

    void render(const std::set<std::shared_ptr<Tile>>& tiles,
                Viewport viewport,
                SDL_Renderer* sdl_renderer);
};


#endif
