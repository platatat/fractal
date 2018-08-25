#include "remote_sink.h"

#include "networking.h"

namespace networking {

RemoteSink::RemoteSink(int socket_fd, std::shared_ptr<tilestream::Source> tile_source) : socket_fd(socket_fd), thread(receiveTask, this, socket_fd, tile_source) {
    display_metrics = true;
    num_requests = 0;
    num_tiles = 0;
    last_report = std::chrono::system_clock::now();
}

void RemoteSink::receive(std::shared_ptr<Tile> tile) {
    {
        std::unique_lock<std::mutex> lock(this->write_mutex);
        sendPacket(socket_fd, tile->getHeader()->serialize());
        std::vector<uint8_t> tile_bytes = networking::serialize(tile->getData());
        sendPacket(socket_fd, tile_bytes);
        this->incMetrics(0, 1);
    }
}

void RemoteSink::receiveTask(RemoteSink* remote_sink, int socket_fd, std::shared_ptr<tilestream::Source> tile_source) {
    while (true) {
        std::vector<uint8_t> header_data = receivePacket(socket_fd);
        if (header_data.size() == 0) break;
        std::shared_ptr<TileHeader> header = TileHeader::deserialize(header_data);

        tile_source->request(std::shared_ptr<TileHeader>(header));
        remote_sink->incMetrics(1, 0);
    }
}

void RemoteSink::incMetrics(int requests, int tiles) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - last_report;
    if (elapsed_seconds.count() > 1) {
        std::cout << "Num requests: " << num_requests << ", num tiles computed: " << num_tiles << std::endl;
        num_requests = 0;
        num_tiles = 0;
        last_report = std::chrono::system_clock::now();
    }

    num_requests += requests;
    num_tiles += tiles;
}

}
