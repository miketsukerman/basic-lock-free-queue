#include <print>

/*
#1
Implement a queue that can hold up to N items of the same data type.

The maximum number of items N and the item data type is specified by the user of the queue.

The queue is used by one producer and on consumer (SPSC), with the producer and consumer executing in its own thread
in the same process.

The operations push() and pop() shall not block, but return immediately if the queue if full (push) or empty (pop).

Focus on showing:
- the data structure(s) needed to maintain the item data and the internal state of the queue.
- the operations push() and pop()
- the operations is_empty() and is_full()


#2
Make the queue from #1 lock free.


#3
Adapt the queue from #2 such, that the consumer and the producer can execute in different processes.

Use POSIX API if necessary.


#4
Adapted the queue to handle multiple consumers and a single producer.

*/

#include <iostream>
#include <cstdint>
#include <array>
#include <utility>
#include <atomic>
#include <thread>
#include <chrono>
#include <cmath>

using namespace std::chrono;

using Index = int64_t;

/**
 * @brief Lock free queue implemented using array as a container.
 * 
 * Implementation uses two indexes front and rear (f & r).  
 */
template <typename T, Index N>
class Queue {

    using AtomicIndex = std::atomic<Index>;

public: 

    Queue() : f{-1},r{-1} {}
    ~Queue() = default;

    Queue(Queue &) = delete;
    Queue(Queue &&) = delete;

    void push(T t) {

        if(r == N-1) {
            std::println("queue::push() is full");
            
            if(f > 0) {
                r = -1;
            }

            return;
        }

        if(f == -1 && r == -1) {
            f = 0;
            r = 0;
        } else {
            r++;
        }

        _queue[r] = t;

        std::println("queue::push({}) front={}, rear={}",t , f.load(), r.load());
    }

    std::optional<T> pop() {

        if(f == -1) {
            std::println("queue::pop() queue is empty");
            return std::nullopt;
        }

        if(f == N-1) {
            f = -1;
        } 

        auto elem = _queue[f];

        if(f == r) {
            f = -1;
            r = -1;
        } else {
            f++;
        }

        std::println("queue::pop({}) front={}, rear={}",elem , f.load(), r.load());
            
        return std::make_optional(elem);
    }

    bool is_full() {
        return (r == N - 1 && f == -1);
    }

    bool is_empty() {
        return (f == -1 && r == -1);
    }

    Index size() {
        return _queue.size();
    }

private: 
    std::array<T,N> _queue;

    AtomicIndex f;
    AtomicIndex r;

};

auto main() -> int 
{
    constexpr Index size{100};

    Queue<int, size> queue;

    std::jthread producer([&]() {
        std::println("Producer thread started");

        for(int i = 0; i < 1000000; i++) {
            std::this_thread::sleep_for(50ms);
            queue.push(pow(i,2));
        }
    });

    constexpr auto Consumers = 4;
    std::vector<std::jthread> consumers;
    for(auto i = 0; i < Consumers; i++) {

        consumers.push_back(std::jthread([&]() {
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
        }));
    }

    return 0;
}
