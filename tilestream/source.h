#ifndef __SOURCE_H__
#define __SOURCE_H__

#include "solver/tile_header.h"
#include <memory>

namespace tilestream {

// Interface for something that can asynchronously return tiles when requested
class Source {
public:
    virtual ~Source() = 0;

    // Asynchronously request tiles
    // Block when this source gets backed up solving previously requested tiles
    // Return true if the request was successfully received; false if the caller
    // needs to resend the request somewhere else
    virtual bool request(std::shared_ptr<TileHeader> header) = 0;
};

inline Source::~Source() { }

}

#endif
