cmake_minimum_required(VERSION 3.23)

project(test-task2)

add_executable(solution src/main.cpp)

target_compile_features(solution PUBLIC cxx_std_23)
