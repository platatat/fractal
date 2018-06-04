#ifndef __TILE_HEADER_H__
#define __TILE_HEADER_H__

#include "complex.h"

#include <cmath>
#include <gmpxx.h>
#include <sstream>
#include <memory>
#include <netinet/in.h>
#include <vector>


class TileHeader {
public:
    mpz_class x;
    mpz_class y;
    int32_t z;
    int16_t i;

    // Delete copy constructor.
    TileHeader(const TileHeader&) = delete;

    TileHeader(mpz_class x_in, mpz_class y_in, uint32_t z_in, int16_t i);

    complex getOrigin() const;

    double getSize() const;

    // TODO: seems cleaner to do hashing/comparison with lambdas.
    struct Hasher {
        std::size_t operator()(const std::shared_ptr<TileHeader>& header) const {
            size_t h = 0;
            // TODO: smarter way of hashing big ints would be good.
            h = header->x.get_si() + (h << 6) + (h << 16) - h;
            h = header->y.get_si() + (h << 6) + (h << 16) - h;
            h = header->z + (h << 6) + (h << 16) - h;
            h = header->i + (h << 6) + (h << 16) - h;
            return h;
        }
    };

    struct Comparator {
        bool operator()(const std::shared_ptr<TileHeader>& a, const std::shared_ptr<TileHeader>& b) const {
            return (a->x == b->x) && (a->y == b->y) && (a->z == b->z) && (a->i == b->i);
        }
    };

    bool operator==(const TileHeader& other) const;

    std::vector<uint8_t> serialize() const;

    static std::shared_ptr<TileHeader> deserialize(const std::vector<uint8_t>& data);

    std::string get_str() const;
};


#endif
