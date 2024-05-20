cmake_minimum_required(VERSION 3.10)
project(static_vector)

set(TARGET sv_demo)
set(TARGET_UT sv_unittest)
set(TARGET_BM sv_benchmark)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_PREFIX_PATH "/home/weber/.local/lib/cmake")

set(SOURCES
        sv_demo.cpp
)

set(HEADERS
        static_vector.h
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O3")

set(BENCHMARK_ENABLE_TESTING OFF)

find_package(GTest REQUIRED)
find_package(benchmark)

add_executable(${TARGET} ${SOURCES} ${HEADERS})
add_executable(${TARGET_UT} static_vector.unittest.cpp)
add_executable(${TARGET_BM} static_vector.benchmark.cpp)

target_link_libraries(${TARGET_UT} GTest::gtest_main)
target_link_libraries(${TARGET_BM} benchmark::benchmark_main)
