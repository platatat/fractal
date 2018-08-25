#include "remote_source.h"

#include "networking.h"

namespace networking {

RemoteSource::RemoteSource(int socket_fd, std::shared_ptr<tilestream::Sink> tile_sink) : socket_fd(socket_fd), thread(receiveTask, socket_fd, tile_sink) {
}

bool RemoteSource::request(std::shared_ptr<TileHeader> header) {
    {
        std::unique_lock<std::mutex> lock(this->write_mutex);
        std::vector<uint8_t> data = header->serialize();
        sendPacket(socket_fd, data);
        return true;
    }
}

void RemoteSource::receiveTask(int socket_fd, std::shared_ptr<tilestream::Sink> tile_sink) {
    while (true) {
        std::vector<uint8_t> header_data = receivePacket(socket_fd);
        if (header_data.size() == 0) break;

        std::shared_ptr<TileHeader> header = TileHeader::deserialize(header_data);
        std::vector<uint8_t> tile_bytes = receivePacket(socket_fd);
        std::vector<uint16_t> tile_data = deserializeVector16(tile_bytes);

        tile_sink->receive(std::shared_ptr<Tile>(new Tile(header, tile_data)));
    }

    tile_sink->close();
}

}
