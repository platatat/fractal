#include "worker.h"

#include "solver/solver.h"

Worker::Worker(int max_work_queue_size) : max_work_queue_size(max_work_queue_size) {
}

void Worker::run(int num_threads) {
    std::cout << "Starting worker with " << num_threads << " threads" << std::endl;
    for (int i = 0; i < num_threads - 1; i++) {
        this->worker_threads.emplace_back(workForever, this);
    }
    workForever(this);
}

bool Worker::request(std::shared_ptr<TileHeader> header) {
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        while ((int) this->work_queue.size() >= max_work_queue_size) {
            this->has_work_space.wait(lock);
        }
        this->work_queue.push_back(header);
        this->has_work.notify_all();
    }

    return true;
}

void Worker::workForever(Worker* worker) {
    while (true) {
        std::shared_ptr<TileHeader> header;
        {
            std::unique_lock<std::mutex> lock(worker->mutex);
            while (worker->work_queue.empty()) {
                worker->has_work.wait(lock);
            }
            header = worker->work_queue[0];
            worker->work_queue.erase(worker->work_queue.begin());
            worker->has_work_space.notify_all();
        }

        std::vector<uint16_t> tile_data = Solver::solveTile(header);
        std::shared_ptr<Tile> tile(new Tile(header, tile_data));
        worker->sink->receive(tile);
    }
}

void Worker::setSink(std::shared_ptr<tilestream::Sink> sink) {
    this->sink = sink;
}
