#ifndef __WORKER_H__
#define __WORKER_H__

#include "solver/tile_header.h"
#include "tilestream/sink.h"
#include "tilestream/source.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Worker : public tilestream::Source {
public:
    Worker(int max_work_queue_size);

    void run(int num_threads);

    bool request(std::shared_ptr<TileHeader> header);
    void setSink(std::shared_ptr<tilestream::Sink> sink);

private:
    int max_work_queue_size;
    std::vector<std::shared_ptr<TileHeader>> work_queue;
    std::vector<std::thread> worker_threads;
    std::shared_ptr<tilestream::Sink> sink;

    std::mutex mutex;
    std::condition_variable has_work;
    std::condition_variable has_work_space;

    static void workForever(Worker* worker);
};

#endif
