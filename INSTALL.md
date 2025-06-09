# Installation Instructions

## Standalone Installation

To install the `static_vector` library, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/dimaweber/static_vector.git
   cd static_vector
   ```

2. **Create a build directory and configure the project with CMake**:
   ```bash
   cmake -S . -B build
   ```

3. **Build the library**:
   ```bash
   cmake --build build
   ```

4. **Install the library** (optional, requires administrator privileges):
   You can specify a custom installation path using `CMAKE_INSTALL_PREFIX`:
   ```bash
   sudo cmake --install build --prefix /path/to/custom/install/dir
   ```
   This will install the headers to your specified directory.

## Using as a CMake Subdirectory

To use `static_vector` in your project as a subdirectory, follow these steps:

1. **Add the repository as a subdirectory**:
   Add this to your top-level `CMakeLists.txt`:
   ```cmake
   add_subdirectory(path/to/static_vector)
   ```

2. **Link against the library**:
   In your target definition, link against `static_vector`:
   ```cmake
   add_executable(my_executable main.cpp)
   target_link_libraries(my_executable PRIVATE static_vector)
   ```

3. **Include the headers in your source files**:
   ```cpp
   #include <static_vector.hxx>
   ```

4. **Build your project**:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

## Using with Standalone Installation

If you've installed `static_vector` using the standalone installation method, you can use it in your projects by:

1. Setting up CMake to find the installed library:
   ```cmake
   find_package(static_vector CONFIG REQUIRED)
   ```

2. Linking against the library:
   ```cmake
   add_executable(my_executable main.cpp)
   target_link_libraries(my_executable PRIVATE wbr::static_vector)
   ```

3. Including the headers in your source files:
   ```cpp
   #include "static_vector.hxx"
   ```

4. Building your project:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```
