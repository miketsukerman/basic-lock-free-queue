#include <LockFreeQueue.h>

#include <thread>
#include <chrono>

using namespace std::chrono;

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Using lock free queue from multiple threads","[multi-threaded]") {

    spdlog::set_pattern("[th %t] %v");
    spdlog::set_level(spdlog::level::trace);

    constexpr Index size{100};
    constexpr uint16_t iterations{size};

    Queue<int, size> q;

    SECTION("Single producer and single consumer") {
        std::thread producer([&]() {
            spdlog::trace("Producer: thread started");

            for(uint16_t i = 0; i < iterations; i++) {
                // sleep added to slow down the production
                std::this_thread::sleep_for(50ms);

                auto r = q.push(i);

                if(r) {
                    spdlog::trace("Producer: pushed {} to the queue", i);
                }

                spdlog::trace("Producer: queue size = {}", q.size());
            }
        });

        std::vector<int> tmp;

        std::thread consumer([&]() {
            spdlog::trace("Consumer: thread started");

            for(uint16_t i = 0; i < iterations+1; i++) {
                auto e = q.pop();

                std::this_thread::sleep_for(200ms);

                if(!e.has_value()) {
                    spdlog::trace("Consumer: queue is empty");
                    continue;
                }

                tmp.emplace_back(*e);

                spdlog::trace("Consumer: fetched element {} from the queue.", *e);

                spdlog::trace("Consumer: queue size = {}", q.size());
            }
        });

        producer.join();
        consumer.join();

        spdlog::trace("Final queue state: {}", q.size());

        spdlog::trace("tmp = {}", tmp.size());

        REQUIRE(tmp.size() == size);
    }


    SECTION("Single producer and multiple consumers") {
        std::thread producer([&]() {
            spdlog::trace("Producer: thread started");

            for(uint16_t i = 0; i < iterations; i++) {
                // sleep added to slow down the production
                std::this_thread::sleep_for(50ms);

                auto r = q.push(i);

                if(r) {
                    spdlog::trace("Producer: pushed {} to the queue", i);
                }

                spdlog::trace("Producer: queue size = {}", q.size());
            }
        });

        Queue<int, size> tmp;
        auto consumer_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> consumers{};

        for(auto cth = 0; cth < consumer_threads; cth++) {
            consumers.emplace_back([&]() {
                spdlog::trace("Consumer: thread started");

                for(uint16_t i = 0; i < iterations + 1; i++) {
                    auto e = q.pop();

                    std::this_thread::sleep_for(200ms);

                    if(!e.has_value()) {
                        spdlog::trace("Consumer: queue is empty");
                        continue;
                    }

                    auto r = tmp.push(*e);

                    spdlog::trace("Consumer: fetched element {} from the queue.", *e);

                    spdlog::trace("Consumer: queue size = {}", q.size());
                }
            });
        }

        producer.join();

        for(auto & consumer: consumers) {
            consumer.join();
        }

        spdlog::trace("Final queue state: {}", q.size());

        spdlog::trace("tmp = {}", tmp.size());

        REQUIRE(tmp.size() == size);
    }
}
