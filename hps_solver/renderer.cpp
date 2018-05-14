#include "renderer.h"
#include <iostream>

#include <math.h>

using std::chrono::system_clock;
using std::chrono::time_point;


Renderer::Renderer() {
    _colored_buffer = new uint8_t [Constants::TILE_PIXELS * 3];
}


Renderer::~Renderer() {
    delete[] _colored_buffer;
}


void Renderer::render(const std::set<std::shared_ptr<Tile>>& tiles, Viewport viewport, SDL_Renderer* sdl_renderer) {
    double translate_x = -viewport.partial_x * (Constants::TILE_WIDTH);
    double translate_y = -viewport.partial_y * (Constants::TILE_HEIGHT);

    for (std::shared_ptr<Tile> tile : tiles) {
        std::shared_ptr<TileHeader> header = tile->getHeader();

        mpz_class mp_tile_x = header->x - viewport.origin_x;
        mpz_class mp_tile_y = header->y - viewport.origin_y;

        int tile_x = mp_tile_x.get_si();
        int tile_y = mp_tile_y.get_si();

        double tile_screen_x = (translate_x + tile_x * Constants::TILE_WIDTH) * viewport.partial_zoom;
        double tile_screen_y = (translate_y + tile_y * Constants::TILE_HEIGHT) * viewport.partial_zoom;

        if (tile->hasData()) {
            SDL_Texture* tile_texture;

            if (cacheContains(header)) {
                auto cache_result = _cache.find(header);
                cache_result->second.last_hit = system_clock::now();
                tile_texture = cache_result->second.texture;
            }
            else {
                tile_texture = createTextureForTile(tile, sdl_renderer);
                cacheInsert(header, tile_texture);
            }

            SDL_Rect dst_rect;
            dst_rect.x = std::floor(tile_screen_x);
            dst_rect.y = std::floor(tile_screen_y);
            dst_rect.w = std::ceil(Constants::TILE_WIDTH * viewport.partial_zoom);
            dst_rect.h = std::ceil(Constants::TILE_HEIGHT * viewport.partial_zoom);

            SDL_RenderCopy(sdl_renderer, tile_texture, NULL, &dst_rect);
        }
    }
}


SDL_Texture* Renderer::createTextureForTile(std::shared_ptr<Tile> tile, SDL_Renderer* sdl_renderer) {
    std::vector<uint16_t> tile_data = tile->getData();

    for (int i = 0; i < Constants::TILE_PIXELS; i++) {
        SDL_Color color = cyclicColor(tile_data[i], tile->getHeader()->iter_lim);
        _colored_buffer[i * 3 + 0] = color.b;
        _colored_buffer[i * 3 + 1] = color.g;
        _colored_buffer[i * 3 + 2] = color.r;
    }

    SDL_Surface* tile_surface = SDL_CreateRGBSurfaceFrom(
        (void*) _colored_buffer,
        Constants::TILE_WIDTH,
        Constants::TILE_HEIGHT,
        24,
        Constants::TILE_WIDTH * 3,
        0, 0, 0, 0
    );

    SDL_Texture* tile_texture = SDL_CreateTextureFromSurface(sdl_renderer, tile_surface);
    SDL_FreeSurface(tile_surface);

    return tile_texture;
}


void Renderer::cacheInsert(std::shared_ptr<TileHeader> header, SDL_Texture* texture) {
    while (_cache.size() >= Constants::TEXTURE_CACHE_SIZE - 1) {
        cacheEvictOldest();
    }

    _cache[header] = {texture, system_clock::now()};
}


bool Renderer::cacheContains(std::shared_ptr<TileHeader> header) {
    return _cache.find(header) != _cache.end();
}


void Renderer::cacheEvictOldest() {
    time_point<system_clock> oldest_time = system_clock::now();
    std::shared_ptr<TileHeader> oldest_header;
    SDL_Texture* oldest_texture;

    for (std::pair<std::shared_ptr<TileHeader>, CachedTexture> element : _cache) {
        if (element.second.last_hit < oldest_time) {
            oldest_texture = element.second.texture;
            oldest_time = element.second.last_hit;
            oldest_header = element.first;
        }
    }

    SDL_DestroyTexture(oldest_texture);
    _cache.erase(oldest_header);
}


SDL_Color Renderer::cyclicColor(int16_t iterations, int16_t iter_lim) {
    double cycle_period = 20;
    double phase = iterations / cycle_period;

    SDL_Color color;

    color.r = sin(sqrt(iterations + 20) * 0.25 + (4 * M_PI / 3)) * 128 + 127;
    color.g = sin(sqrt(iterations + 20) * 0.25 + (2 * M_PI / 3)) * 128 + 127;
    color.b = sin(sqrt(iterations + 20) * 0.25 + (0 * M_PI / 3)) * 128 + 127;

    if (iterations == iter_lim - 1) {
        color.r = 255;
        color.g = 255;
        color.b = 255;
    }

    return color;
}
