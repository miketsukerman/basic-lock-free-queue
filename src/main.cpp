#include <print>

class Solution {
public:
    Solution() {}

    auto run() -> void {
        std::println("Hello, world!");
    }
};

auto main() -> int 
{
    Solution solution;

    solution.run();

    return 0;
}
