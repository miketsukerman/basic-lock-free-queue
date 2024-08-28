#pragma once

/**
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

#include <cstdint>
#include <array>
#include <atomic>

#include <spdlog/spdlog.h>

using Index = uint64_t;

/**
 * @brief Lock free queue implemented using array as a container.
 *
 * Implementation uses two indexes pop and push (f & r).
 */
template <typename T, Index N>
class Queue {

    using AtomicIndex = std::atomic<Index>;

public:

    Queue() : _arr{} {}
    ~Queue() = default;

    /**
     * @brief adds element of type T to the queue
     * @returns
     * - true when queue accepted new element
     * - false when queue is full
     */
    [[nodiscard]] auto push(T const & t) {

        if(is_full()) {
            spdlog::trace("queue::push(): queue is full");

            return false;
        }

        auto i = _r % capacity();
        _arr[i] = t;

        spdlog::trace("queue::push({}) i={}, front={}, rear={}",t , i, _f.load(), _r.load());

        ++_r;

        return true;
    }

    /**
     * @brief pops element of type T from the queue
     *
     * @return
     * - std::optional<T> if queue is not empty
     * - std::nullopt if queue is empty
     */
    std::optional<T> pop() {

        if(is_empty()) {
            spdlog::trace("queue::pop(): queue is empty");

            // if queue is empty we return std::nullopt
            return std::nullopt;
        }

        auto i = _f % capacity();
        auto elem = _arr[i];

        spdlog::trace("queue::pop({}) i={}, front={}, rear={}",elem , i, _f.load(), _r.load());

        ++_f;

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
    [[nodiscard]] auto is_full() const noexcept {
        return (size() == capacity());
    }

    /**
     * @brief checks if queue is empty
     *
     * @returns
     * - true if queue is empty
     * - false if there are some elements
     */
    [[nodiscard]] auto is_empty() const noexcept {
        return (size() == 0);
    }

    /**
     * @brief Returns size of the queue (number of elements at the moment)
     */
    [[nodiscard]] auto size() const noexcept {
        return _r - _f;
    }

    /**
     * @brief Returns capacity of the queue (maximum number of elements)
     */
    [[nodiscard]] constexpr auto capacity() const noexcept {
        return _arr.size();
    }

private:
    std::array<T,N> _arr;

    AtomicIndex _f;
    AtomicIndex _r;

};
