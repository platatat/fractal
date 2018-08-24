#include "tile_header.h"


TileHeader::TileHeader(mpz_class x_in, mpz_class y_in, uint32_t z_in, int16_t i) : 
    x(x_in), y(y_in), z(z_in), i(i) {
};


complex TileHeader::getOrigin() const {
    mpf_class f_header_x(x, z + 64);
    mpf_class f_header_y(y, z + 64);
    double size = getSize();
    return {f_header_x * size, f_header_y * size};
}


double TileHeader::getSize() const {
    // TODO: precision
    return pow(2.0, (double) -z);
}


bool TileHeader::operator==(const TileHeader& other) const {
    return (x == other.x) && (y == other.y) && (z == other.z) && i == other.i;
}


std::vector<uint8_t> TileHeader::serialize() const {
    std::string x_str = x.get_str();
    std::string y_str = y.get_str();

    uint32_t x_size = x_str.length() + 1;
    uint32_t y_size = y_str.length() + 1;
    int serialized_size = x_size + y_size + sizeof(x_size) + sizeof(y_size) + sizeof(z) + sizeof(i);

    uint8_t buffer [serialized_size];

    const uint8_t* x_cstr = (uint8_t*) x_str.c_str();
    const uint8_t* y_cstr = (uint8_t*) y_str.c_str();

    // Put x_size and y_size into buffer.
    ((uint32_t*) buffer)[0] = htonl(x_size);
    ((uint32_t*) buffer)[1] = htonl(y_size);
    int offset = 8;

    // Put x and y into buffer.
    std::memcpy(buffer + offset, x_cstr, x_size);
    offset += x_size;

    std::memcpy(buffer + offset, y_cstr, y_size);
    offset += y_size;

    // Put z into buffer.
    *((uint32_t*) (buffer + offset)) = htonl(z);
    offset += sizeof(z);

    // Put i into buffer
    *((int16_t*) (buffer + offset)) = htons(i);

    std::vector<uint8_t> result(buffer, buffer + serialized_size);
    return result;
}


std::shared_ptr<TileHeader> TileHeader::deserialize(const std::vector<uint8_t>& data) {
    const uint8_t* buffer = &data[0];

    // Read x_size and y_size from buffer.
    uint32_t x_size = ntohl(((uint32_t*) buffer)[0]);
    uint32_t y_size = ntohl(((uint32_t*) buffer)[1]);
    int offset = 8;

    // Make sure buffer is correct size.
    if (data.size() != x_size + y_size + sizeof(x_size) + sizeof(y_size) + sizeof(z) + sizeof(i)) {
        std::cout << x_size << ", " << y_size << ", " << data.size() << std::endl;
        throw std::runtime_error("tile header deserialization size mismatch\n");
    }

    // Read x and y from buffer.
    mpz_class x = mpz_class((char*) (buffer + offset));
    offset += x_size;
    mpz_class y = mpz_class((char*) (buffer + offset));
    offset += y_size;

    // Read z from buffer;
    uint32_t z = ntohl(*((uint32_t*) (buffer + offset)));
    offset += sizeof(z);

    // Read i from buffer
    int16_t i = ntohs(*((int16_t*) (buffer + offset)));

    return std::make_shared<TileHeader>(x, y, z, i);
}


std::string TileHeader::get_str() const {
    std::ostringstream ss;
    ss << "(" << x.get_str() << ", " << y.get_str() << ", " << z << ", " << i << ")";
    return ss.str();
}
