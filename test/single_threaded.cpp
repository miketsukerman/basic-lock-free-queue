#include <LockFreeQueue.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Using lock free queue from one thread","[single-threaded]") {

    spdlog::set_pattern("[th %t] %v");
    spdlog::set_level(spdlog::level::trace);

    constexpr Index size{10};

    std::array<int,size> tmp{0,1,2,3,4,5,6,7,8,9};
    Queue<int, size> q;

    SECTION("Push and pop from one thread") {

        for(auto v: tmp) {
            auto r = q.push(v);
            REQUIRE(r == true);
        }

        REQUIRE(q.size() == size);
        REQUIRE(q.is_empty() == false);
        REQUIRE(q.is_full() == true);

        for(size_t i=0; i < size; i++) {
            auto v = q.pop();
            REQUIRE(v.has_value());
            REQUIRE(i == v);
        }
    }

    SECTION("Make queue empty") {

        for(auto v: tmp) {
            auto r = q.push(v);
            REQUIRE(r == true);
        }

        REQUIRE(q.size() == size);

        for(size_t i=0; i < size+5; i++) {
            auto v = q.pop();
            if(i > size) {
                REQUIRE(v.has_value() == false);
            }
        }
    }

    SECTION("Try to push more items then queue can accept") {
        for(int i=0; i < size*2; i++) {
            auto r = q.push(i);

            if(i < size) {
                REQUIRE(r == true);
            } else {
                REQUIRE(r == false);
            }
        }

        REQUIRE(q.size() == size);
    }
}
