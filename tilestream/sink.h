#ifndef __TILE_SINK_H__
#define __TILE_SINK_H__

#include "solver/tile.h"
#include <memory>

namespace tilestream {

// Interface for something that can asynchronously receive tiles when produced
class Sink {
public:
    virtual ~Sink() = 0;

    // Asynchronously receive tiles
    // The sink implementation is responsible for ensure that this is thread safe
    virtual void receive(std::shared_ptr<Tile> tile) = 0;

    virtual void close() {};
};

inline Sink::~Sink() { }

}

#endif
