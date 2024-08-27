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
 * Implementation uses two indexes pop and push (f & r).  
 */
template <typename T, Index N>
class Queue {

    using AtomicIndex = std::atomic<Index>;

public: 

    Queue() : _arr{},f{-1},r{-1} {}
    ~Queue() = default;

    /**
     * @brief adds element of type T to the queue
     */
    void push(const T & t) {

        // r (push index) reached the end of the array
        if(r == size() - 1) {
            std::println("queue::push() is full");
            
            // if f (pop index) is not zero 
            // meaning that we removed some 
            // elements from the queue
            // then reset push index r to the
            // beginning of the array
            if(f > 0) {
                r = -1;
            }

            return;
        }

        // if both push and pop indexes are -1 
        // means queue is empty
        if(f == -1 && r == -1) {
            f = 0;
            r = 0;
        } else {
            // otherwise increment push index
            ++r;
        }

        // copy element to the array 
        _arr[r] = t;

        std::println("queue::push({}) front={}, rear={}",t , f.load(), r.load());
    }

    /**
     * @brief pops element of type T from the queue
     * 
     * @return 
     * - std::optional<T> if queue is not empty
     * - std::nullopt if queue is empty
     */
    std::optional<T> pop() {

        // if pop index f is -1 means queue is empty
        if(f == -1) {
            std::println("queue::pop() queue is empty");

            // if queue is empty we return std::nullopt
            return std::nullopt;
        }

        // picking up element at position f
        auto elem = _arr[f];

        // if we reached the end of the array 
        // reset pop index f to -1 (pre beginning position)
        if(f == size() - 1) {
            f = -1;
        } 

        // if pop index is equal to push index (r == f) 
        // it means there are no elements in the queue
        if(f == r) {
            std::println("queue::pop() queue is empty");
            return std::nullopt;
        } else {
            // otherwise we increment pop index f
            ++f;
        }

        std::println("queue::pop({}) front={}, rear={}",elem , f.load(), r.load());
            
        // returning element
        return std::make_optional(elem);
    }

    /**
     * @brief checks if queue is full
     * 
     * @returns 
     * - true if queue container is full
     * - false if there is a space for new elements
     */
    bool is_full() {
        return (r == size() - 1 && f == -1);
    }

    /**
     * @brief checks if queue is empty
     * 
     * @returns 
     * - true if queue container is empty
     * - false if there are some elements 
     */
    [[nodiscard]] bool is_empty() const {
        return (f == -1 && r == -1);
    }

    /**
     * @brief Returns size of the queue container
     */
    constexpr Index size() {
        return _arr.size();
    }

private: 
    std::array<T,N> _arr;

    AtomicIndex f;
    AtomicIndex r;

};

auto main() -> int
{
    constexpr Index size{100};

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
            queue.push(pow(i,2));
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
