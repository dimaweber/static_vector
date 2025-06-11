cmake_minimum_required(VERSION 3.10)
project(static_vector VERSION 0.2 DESCRIPTION "stack-allocated vector with standard api")

include(GNUInstallDirs)

set(TARGET_LIB static_vector)
set(TARGET_DM sv_demo)
set(TARGET_UT sv_unittest)
set(TARGET_BM sv_benchmark)

set(CMAKE_CXX_STANDARD 20)

#list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local/lib/cmake)

set(HEADERS
        include/static_vector.hxx
        include/static_string.hxx
        include/bound_check.hxx
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O3")

OPTION(BUILD_BENCHMARK "static_vector benchmark" ON)
OPTION(BUILD_TESTS "static_vector test" ON)
OPTION(BUILD_EXAMPLES "static_vector examples" ON)
OPTION(BUILD_DOCS "static_vector documentation" ON)

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(BUILD_TESTS OFF)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_BENCHMARK OFF)
    set(BUILD_DOCS OFF)
endif ()

if (BUILD_TESTS)
    add_compile_definitions(BUILD_TESTS=1)
else ()
    add_compile_definitions(BUILD_TESTS=0)
endif ()

add_library(${TARGET_LIB} INTERFACE
        include/bound_check.hxx)
target_include_directories(${TARGET_LIB}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


if (BUILD_TESTS)

    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(${TARGET_UT}
            tests/static_vector.unittest.cpp
            tests/static_vector_adapter.unittest.cxx
            tests/static_string.unittest.cxx
    )

    include(GoogleTest)
    gtest_discover_tests(${TARGET_UT})
    target_link_libraries(${TARGET_UT} PRIVATE GTest::gtest_main ${TARGET_LIB})
else ()
    add_compile_definitions(BUILD_TESTS=0)
endif ()

if (BUILD_BENCHMARK)
    find_package(benchmark)

    add_executable(${TARGET_BM} benchmark/static_vector.benchmark.cpp)

    target_link_libraries(${TARGET_BM} PRIVATE ${TARGET_LIB} benchmark::benchmark_main)
endif ()

if (BUILD_EXAMPLES)
    find_package(fmt 11)
    add_executable(${TARGET_DM}
            examples/base_usage.cpp
            examples/range-based-for-loop.cpp
            examples/sv_demo.cpp
            examples/standard-algorithm-functions.cpp
    )
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
        doxygen_add_docs(doc_doxygen ALL
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                COMMENT "Generating API documentation with Doxygen"
        )
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
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
unset(EXPORT_TARGETS)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)


install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/static_vector-config-version.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}
)

install(TARGETS ${TARGET_LIB}
        EXPORT wbr-targets
        #DESTINATION ${INSTALL_CMAKE_DIR}
        #FILE_SET HEADERS
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT wbr-targets
        NAMESPACE wbr::
        DESTINATION ${INSTALL_CMAKE_DIR}
)
