cmake_minimum_required(VERSION 3.25)

project(LockFreeQueue)

# add dependencies
include(cmake/CPM.cmake)

option(ENABLE_TESTS "Enables build of tests" ON)

if(ENABLE_TESTS)
CPMAddPackage("gh:catchorg/Catch2@3.7.0")
endif()

add_library(queue INTERFACE)
target_include_directories(queue INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(queue INTERFACE cxx_std_23)

# example usage
add_executable(solution src/main.cpp)

target_compile_features(solution PUBLIC cxx_std_23)
target_link_libraries(solution PUBLIC queue)

# tests
if(ENABLE_TESTS)
add_executable(tests
        test/single_threaded.cpp
        )
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain queue)


list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

catch_discover_tests(tests)
endif()