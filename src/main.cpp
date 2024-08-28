#include <cmath>
#include <thread>
#include <chrono>

#include <LockFreeQueue.h>

using namespace std::chrono;

auto main() -> int
{
    spdlog::set_pattern("[th %t] %v");
    spdlog::set_level(spdlog::level::trace);

    constexpr Index size{20};

    Queue<int, size> queue;

    /**
    * this is a single producer thread 
    * pushing some arbitrary integers to 
    * the queue
    */
    std::jthread producer([&]() {
        spdlog::trace("Producer thread started");

        for(int i = 0; i < 1000000; i++) {
            // sleep added to slow down the production
            std::this_thread::sleep_for(50ms);
            auto r = queue.push(std::pow(i,2));
        }
    });

    
    /**
     * Pool of consumer threads 
     */
    constexpr auto Consumers = 4;
    std::vector<std::jthread> consumers;
    
    /**
     * creating consumer threads in a loop
     */
    for(auto i = 0; i < Consumers; i++) {

        consumers.emplace_back([&]() {
            size_t ci = i;
            spdlog::trace("Consumer{} thread started",ci);

            while(true) {
                auto e = queue.pop();

                if(!e.has_value()) {
                    spdlog::trace("Consumer{}: queue is empty",ci);
                    std::this_thread::sleep_for(200ms);
                    continue;
                }

                std::this_thread::sleep_for(200ms);
                spdlog::trace("Consumer{}: element {} from queue.",ci, *e);
            }
        });
    }

    return 0;
}
