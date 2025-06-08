# static_vector

Stack-allocated vector that mimics std::vector interface.

### Tested for compatibility:

- standards:
    - ~~c++17~~
    - c++20
    - c++23
- compilers:
    - icc-2024.1
    - gcc-10
    - gcc-11
    - gcc-12
    - gcc-13
    - gcc-14
    - clang-15 (partial, benchmark fails to compile)
    - clang-16
    - clang-17 (partial, benchmark fails to compile)
    - clang-18

---

Static Vector provides two C++ template classes that offer functionality similar to `std::vector` but with fixed capacity:

1. **static_vector**: A container that stores data in internal memory allocated on the stack.
2. **static_vector_adapter**: A non-owning wrapper over existing C-arrays or `std::array`.

Both classes attempt to mimic the `std::vector` API as closely as possible, with limitations related to their fixed-size nature.

## Features

- Fixed capacity containers
- Customizable bounds checking (Exception, Assertion, LimitToBound, or NoCheck)
- Memory-efficient
- Compatible with C++20 and above
- Comprehensive test suite using Google Test framework
- Benchmarks using Google Benchmark

## Build Requirements

- CMake 3.10 or higher
- C++20 compatible compiler (GCC, Clang, etc.)
- Google Test framework (for unit tests)
- Google Benchmark library (for performance testing)
- Doxygen (optional, for documentation generation)

## Building the Project

```bash
  cmake -S. -B build
  cmake --build build
``` 

## Components

The project consists of several components:

### static_vector.hxx

The main header file containing both `static_vector` and `static_vector_adapter` class template definitions.

### sv_demo.cpp

A simple demonstration program showcasing basic usage of the vector classes.

### Unit Tests (static_vector.unittest.cpp)

Comprehensive unit tests using Google Test framework to verify functionality.

```bash
  cmake -S. -B build
  cmake --build build --target sv_unittest
  ctest --test-dir build
```

### Benchmarks (static_vector.benchmark.cpp)

Performance benchmarks using Google Benchmark library.

## Container Types

The project offers two types of containers:

1. **`static_vector`**: A container that stores data in internal memory allocated on the stack.
2. **`static_vector_adapter`**: A non-owning wrapper over existing C-arrays or `std::array`.

Both classes attempt to mimic the `std::vector` API as closely as possible, with limitations related to their fixed-size nature.

## Bound Checking Strategies

Both classes support different bound checking strategies, controlled by the `BoundCheckStrategy` enum:

- **Exception**: Throws an exception if bounds are violated
    - Throws `std::overflow_error` when vector is full
    - Throws `std::out_of_range` for invalid iterator positions
- **Assert**: Uses assertions to check bounds (debug mode only)
- **LimitToBound**: Silently limits the position to valid range without throwing
- **NoCheck** (UB): No checks are performed; bound violations lead to undefined behavior

## Documentation

API documentation can be generated using Doxygen. If Doxygen is found during configuration, it will generate HTML documentation in the build directory.

```bash
  cmake -S. -B build -DBUILD_DOCS=ON  
  cmake --build build --target doc_doxygen
``` 

The generated documentation will be available in `build/docs/html`.

## License

This project is licensed under the Creative Commons License.
