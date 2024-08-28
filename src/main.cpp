#include <cmath>

#include <LockFreeQueue.h>

auto main() -> int
{
    constexpr Index size{20};

    Queue<int, size> queue;

    /**
    * this is a single producer thread 
    * pushing some arbitrary integers to 
    * the queue
    */
    std::jthread producer([&]() {
        std::println("Producer thread started");

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
            std::println("Consumer{} thread started",ci);

            while(true) {
                auto e = queue.pop();

                if(!e.has_value()) {
                    std::println("Consumer{}: queue is empty",ci);
                    std::this_thread::sleep_for(200ms);
                    continue;
                }

                std::this_thread::sleep_for(200ms);
                std::println("Consumer{}: element {} from queue.",ci, *e);
            }
        });
    }

    return 0;
}
