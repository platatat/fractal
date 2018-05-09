#ifndef __TILE_HEADER_H__
#define __TILE_HEADER_H__

#include <gmpxx.h>
#include <iostream>
#include <sstream>
#include <memory>


class TileHeader {
public:
    mpz_class x;
    mpz_class y;
    long z;

    // Delete copy constructor.
    TileHeader(const TileHeader&) = delete;

    TileHeader(mpz_class x_in, mpz_class y_in, long z_in) : x(x_in), y(y_in), z(z_in) {};

    bool operator==(const TileHeader& other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    struct Hasher {
        std::size_t operator()(const std::shared_ptr<TileHeader>& header) const {
            size_t h = 0;
            // TODO: smarter way of hashing big ints would be good.
            h = header->x.get_si() + (h << 6) + (h << 16) - h;
            h = header->y.get_si() + (h << 6) + (h << 16) - h;
            h = header->z + (h << 6) + (h << 16) - h;
            return h;
        }
    };

    struct Comparator {
        bool operator()(const std::shared_ptr<TileHeader>& a, const std::shared_ptr<TileHeader>& b) const {
            return (a->x == b->x) && (a->y == b->y) && (a->z == b->z);
        }
    };

    unsigned char* serialize(int& serialized_size) {
        std::string x_str = x.get_str();
        std::string y_str = y.get_str();

        int x_size = x_str.length();
        int y_size = y_str.length();
        serialized_size = x_size + y_size + 16;

        unsigned char* buffer = new unsigned char [serialized_size];

        const unsigned char* x_cstr = (unsigned char*) x_str.c_str();
        const unsigned char* y_cstr = (unsigned char*) y_str.c_str();

        // Put x_size and y_size into buffer.
        ((int*) buffer)[0] = x_size;
        ((int*) buffer)[1] = y_size;
        int offset = 8;

        // Put x and y into buffer.
        std::memcpy(buffer + offset, x_cstr, x_size);
        offset += x_size;

        std::memcpy(buffer + offset, y_cstr, y_size);
        offset += y_size;

        // Put z into buffer.
        *((long*) (buffer + offset)) = z;

        return buffer;
    }

    static std::unique_ptr<TileHeader> deserialize(const int buffer_size, unsigned char* const buffer) {
        // Read x_size and y_size from buffer.
        int x_size = ((int*) buffer)[0];
        int y_size = ((int*) buffer)[1];
        int serialized_size = x_size + y_size + 16;

        // Make sure buffer is correct size.
        if (buffer_size != serialized_size) {
            throw std::runtime_error("buffer size mismatch");
        }

        int offset = 8;

        // Read x and y from buffer. TODO: see if we can do this in place.
        char x_subbuffer [x_size + 1];
        std::memcpy(x_subbuffer, buffer + offset, x_size);
        x_subbuffer[x_size] = '\0';
        mpz_class x = mpz_class(x_subbuffer);
        offset += x_size;

        char y_subbuffer [y_size + 1];
        std::memcpy(y_subbuffer, buffer + offset, y_size);
        y_subbuffer[y_size] = '\0';
        mpz_class y = mpz_class(y_subbuffer);
        offset += y_size;

        // Read z from buffer;
        long z = *((long*) (buffer + offset));

        return std::make_unique<TileHeader>(x, y, z);
    }

    std::string get_str() {
        std::ostringstream ss;
        ss << "(" << x.get_str() << ", " << y.get_str() << ", " << z << ")";
        return ss.str();
    }
};


#endif
