cmake_minimum_required(VERSION 3.10)
project(
        static_vector
        VERSION 0.6.1
        DESCRIPTION "stack-allocated vector with standard api")

include(GNUInstallDirs)

set(TARGET_LIB static_vector)
set(TARGET_DM sv_demo)
set(TARGET_UT sv_unittest)
set(TARGET_BM sv_benchmark)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local/lib/cmake)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif ()

set(HEADERS
        include/bound_check.hxx
        include/concepts.hxx
        include/len_string.hxx
        include/static_string.hxx
        include/static_vector.hxx
        include/string_manipulations.hxx
        include/string_manipulations_ext.hxx
        include/system_handler.hxx
        include/text_file_reader.hxx
        include/tlv_vector.hxx
)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")   #-march=raptorlake
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")       # -march=raptorlake

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -O3")      # -march=raptorlake
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O3")  # -march=raptorlake

option(BUILD_BENCHMARK "static_vector benchmark" ON)
option(BUILD_TESTS "static_vector test" ON)
option(BUILD_EXAMPLES "static_vector examples" ON)
option(BUILD_DOCS "static_vector documentation" ON)
option(FMT_SUPPORT "support for fmt lib" ON)
option(STD_FORMAT_SUPPORT "support for std::format (C++20)" OFF)
option(IOSTREAM_SUPPORT "support for iostream" ON)

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(BUILD_TESTS OFF)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_BENCHMARK OFF)
    set(BUILD_DOCS OFF)
endif ()

if (BUILD_TESTS)
    add_compile_definitions(BUILD_TESTS=1)
    set(FMT_SUPPORT ON)
else ()
    add_compile_definitions(BUILD_TESTS=0)
endif ()

if (BUILD_EXAMPLES)
    set(FMT_SUPPORT ON)
endif ()


if (FMT_SUPPORT)
    add_compile_definitions(FMT_SUPPORT=1)
    find_package(fmt 11.2 REQUIRED)
    if (NOT fmt_FOUND)
        FetchContent_Declare(
                fmt
                GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                GIT_TAG 11.2.0
        )
        FetchContent_MakeAvailable(fmt)
    endif ()
else ()
    add_compile_definitions(FMT_SUPPORT=0)
endif ()

if (STD_FORMAT_SUPPORT)
    add_compile_definitions(STD_FORMAT_SUPPORT=1)
else ()
    add_compile_definitions(STD_FORMAT_SUPPORT=0)
endif ()

if (IOSTREAM_SUPPORT)
    add_compile_definitions(IOSTREAM_SUPPORT=1)
else ()
    add_compile_definitions(IOSTREAM_SUPPORT=0)
endif ()

add_library(${TARGET_LIB} INTERFACE ${HEADERS})
target_include_directories(
        ${TARGET_LIB} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    if (NOT GTest_FOUND)
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif ()
    add_executable(
            ${TARGET_UT}
            tests/static_string.unittest.cxx
            tests/static_vector.unittest.cxx
            tests/static_vector_adapter.unittest.cxx
            tests/string_manipulations.unittest.cxx
            tests/string_manipulations_ext.unittest.cxx
            tests/len_string.unittest.cxx
    )

    include(GoogleTest)
    gtest_discover_tests(${TARGET_UT})
    target_link_libraries(${TARGET_UT} PRIVATE GTest::gtest_main ${TARGET_LIB} fmt::fmt)
else ()
    add_compile_definitions(BUILD_TESTS=0)
endif ()

if (BUILD_BENCHMARK)
    find_package(benchmark)

    add_executable(${TARGET_BM} benchmark/static_vector.benchmark.cpp)

    target_link_libraries(${TARGET_BM} PRIVATE ${TARGET_LIB}
            benchmark::benchmark_main)
endif ()

if (BUILD_EXAMPLES)
    add_executable(
            ${TARGET_DM}
            examples/base_usage.cpp examples/range-based-for-loop.cpp
            examples/sv_demo.cpp examples/standard-algorithm-functions.cpp
            examples/tlv_array_base_usage.cpp
            examples/coroutine.cpp
            examples/string_demo.cpp)
    target_link_libraries(${TARGET_DM} PRIVATE ${TARGET_LIB})
    if (fmt_FOUND)
        add_compile_definitions(USE_FMT=1)
        target_link_libraries(${TARGET_DM} PRIVATE fmt::fmt)
    else ()
        add_compile_definitions(USE_FMT=0)
    endif ()
endif ()

if (BUILD_DOCS)
    # Find Doxygen
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        message(STATUS "Doxygen found: generating documentation")
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # Configure and generate Doxyfile
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        doxygen_add_docs(
                doc_doxygen ALL
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                COMMENT "Generating API documentation with Doxygen")
    else (DOXYGEN_FOUND)
        message(STATUS "Doxygen not found: skipping documentation generation")
    endif (DOXYGEN_FOUND)
endif ()

set(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/static_vector)

include(CMakePackageConfigHelpers)
set(EXPORT_TARGETS wbr::static_vector)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/static_vector-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config.cmake
        INSTALL_DESTINATION ${INSTALL_CMAKE_DIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR)
unset(EXPORT_TARGETS)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config-version.cmake
        DESTINATION ${INSTALL_CMAKE_DIR})

install(
        TARGETS ${TARGET_LIB}
        EXPORT wbr-targets
        # DESTINATION ${INSTALL_CMAKE_DIR}
        # FILE_SET HEADERS
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
        EXPORT wbr-targets
        NAMESPACE wbr::
        DESTINATION ${INSTALL_CMAKE_DIR})
