# Build Instructions

## Prerequisites

Make sure you have the following tools installed on your system:

- C++ compiler (e.g., GCC, Clang)
- cmake (version 3.10 or higher)
- git

Optional dependencies for additional features:

- Google Test framework (`GTest`) for unit tests
- Benchmark library for performance benchmarks
- fmt library for formatted output in examples
- Doxygen for generating documentation

## Building the Project

### Step-by-Step Guide

1. **Clone the Repository**

   ```bash
   git clone git@github.com:dimaweber/static_vector.git
   cd static_vector
   ```

2. **Generate Build Files with CMake**

   Run the following command to generate the build files:

   ```bash
   cmake -S. -B build
   ```

3. **Build the Project**

   Use `cmake` or `make` (depending on your system) to compile the project:

   ```bash
   cmake --build build
   ```

## Building Specific Components

The project has several optional components that can be enabled using CMake options:

- **Unit Tests**: Enabled by default, requires Google Test framework.
  ```bash
  cmake -DBUILD_TESTS=ON ..
  ```
- **Benchmarks**: Enabled by default, requires benchmark library.
  ```bash
  cmake -DBUILD_BENCHMARK=ON ..
  ```
- **Examples**: Enabled by default, requires fmt library for formatted output.
  ```bash
  cmake -DBUILD_EXAMPLES=ON ..
  ```
- **Documentation**: Disabled by default, requires Doxygen.
  ```bash
  cmake -DBUILD_DOCS=ON ..
  ```

## Running Tests

To run the tests (if enabled), you can use CTest which is integrated with CMake:

```bash
 ctest --test-dir build
``` 

## Cleaning Up

If you want to clean up the build files and directories, you can simply remove the `build` directory:

```bash
 rm -rf build
``` 

Alternatively, you can use CMake's clean target if you prefer:

```bash
 cmake --build build  --target clean
```
