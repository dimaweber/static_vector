cmake_minimum_required(VERSION 3.10)
project(static_vector)

set(TARGET sv_demo)
set(TARGET_UT sv_unittest)
set(TARGET_BM sv_benchmark)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local/lib/cmake)

set(SOURCES
        sv_demo.cpp
)

set(HEADERS
        static_vector.hxx
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O3")

enable_testing()

find_package(GTest REQUIRED)
find_package(benchmark)

add_executable(${TARGET} ${SOURCES} ${HEADERS})
add_executable(${TARGET_UT} static_vector.unittest.cpp
        static_vector_adapter.unittest.cxx)
add_executable(${TARGET_BM} static_vector.benchmark.cpp)

target_link_libraries(${TARGET_UT} GTest::gtest_main)
target_link_libraries(${TARGET_BM} benchmark::benchmark_main)

include(GoogleTest)
gtest_discover_tests(${TARGET_UT})

# Find Doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND)
    message(STATUS "Doxygen found: generating documentation")
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Configure and generate Doxyfile
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message(STATUS "Doxygen not found: skipping documentation generation")
endif (DOXYGEN_FOUND)
