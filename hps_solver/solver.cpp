#include "solver.h"

#include "constants.h"

#include <iostream>


void Solver::freeListAppend(volatile uint16_t* data) {
    std::unique_lock<std::mutex> lock(mutex);
    free_list.emplace_back(data, [this] (volatile uint16_t* data) { freeListAppend(data); });
    has_space.notify_all();
}


void Solver::sumbit(std::shared_ptr<TileHeader> tile, uint16_t iterations) {
    std::unique_lock<std::mutex> lock(mutex);
    while (free_list.empty()) has_space.wait(lock);

    Solver::data data = std::move(free_list.front());
    free_list.pop_front();
    data[Constants::TILE_WIDTH * Constants::TILE_HEIGHT - 1] = -2;
    solveTile(tile, data, iterations);
    inflight[tile] = std::move(data);
}


Solver::data Solver::retrieve(std::shared_ptr<TileHeader> tile) {
    std::unique_lock<std::mutex> lock(mutex);

    Solver::data& data = inflight[tile];
    if (data == nullptr) return nullptr;
    if (data[Constants::TILE_WIDTH * Constants::TILE_HEIGHT - 1] == -2) return nullptr;
    Solver::data ret = std::move(inflight[tile]);
    inflight.erase(tile);
    return ret;
}
