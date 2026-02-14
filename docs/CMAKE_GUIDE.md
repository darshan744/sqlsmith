# 📘 CMake Learning Guide for SQLsmith

This guide explains every concept used in our CMakeLists.txt file.

## Table of Contents
1. [CMake Basics](#cmake-basics)
2. [Key Concepts](#key-concepts)
3. [Our Project Structure](#our-project-structure)
4. [Line-by-Line Explanation](#line-by-line-explanation)
5. [Common CMake Variables](#common-cmake-variables)
6. [Building the Project](#building-the-project)

---

## CMake Basics

### What is CMake?
- **NOT a build system** - it's a *build system generator*
- Generates platform-specific build files (Makefiles on Linux, .sln on Windows)
- Abstracts away platform differences

### The CMake Workflow
```
CMakeLists.txt → cmake → Makefile → make → executable
     (recipe)     (generator) (build instructions) (compiler) (final program)
```

---

## Key Concepts

### 1. **Targets**
A target is anything CMake builds (executable, library, etc.).

```cmake
add_executable(sqlsmith)  # Creates a target named "sqlsmith"
```

Think of it as a "recipe" for building something.

### 2. **Properties**
Targets have properties (compiler flags, include paths, link libraries).

```cmake
target_sources(sqlsmith PRIVATE src/main.cc)        # Add source files
target_include_directories(sqlsmith PRIVATE include/) # Add include paths
target_link_libraries(sqlsmith PRIVATE PostgreSQL)  # Link with libraries
```

### 3. **Visibility: PRIVATE vs PUBLIC vs INTERFACE**
- **PRIVATE**: Only this target uses it
- **PUBLIC**: This target AND targets that depend on it use it
- **INTERFACE**: Only targets that depend on this use it (not this target itself)

For executables (like sqlsmith), always use **PRIVATE** (executables can't be dependencies).

### 4. **Variables**
CMake has built-in and user-defined variables:

```cmake
set(MY_VAR "hello")              # Create variable
message("Value: ${MY_VAR}")       # Use variable (need ${} syntax)
```

---

## Our Project Structure

```
sqlsmith/
├── CMakeLists.txt           # Build configuration (this file!)
├── src/                     # Source files (.cc)
│   ├── sqlsmith.cc         # Main entry point
│   ├── schema.cc
│   └── ...
├── include/                 # Header files (.hh)
│   ├── schema.hh
│   └── ...
├── cmake/modules/           # Custom CMake modules
│   ├── FindPQXX.cmake
│   └── FeatureTests.cmake
└── build/                   # Build directory (created by you)
    ├── config.h            # Generated config
    ├── gitrev.h            # Generated version
    └── sqlsmith            # Final executable
```

---

## Line-by-Line Explanation

### Project Setup
```cmake
cmake_minimum_required(VERSION 3.14)
```
**Purpose**: Require CMake 3.14+ (has features we need like FetchContent).  
**Why**: Prevents errors from using newer features with old CMake.

```cmake
project(SQLsmith VERSION 1.4.0 DESCRIPTION "..." LANGUAGES CXX)
```
**Purpose**: Declare project metadata.  
**Sets variables**:
- `PROJECT_NAME` = "SQLsmith"
- `PROJECT_VERSION` = "1.4.0"
- `CMAKE_PROJECT_NAME` = "SQLsmith" (if top-level project)

**LANGUAGES CXX**: Tells CMake to find a C++ compiler.

---

### Compiler Configuration
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

**What it does**:
- Use C++17 standard (`-std=c++17`)
- Don't fall back to C++14/11 if compiler doesn't support C++17
- Don't use GNU extensions (`-std=c++17`, not `-std=gnu++17`)

**Why**: Ensures consistent, portable C++17 code.

---

### Finding Custom Modules
```cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
```

**What it does**: Tells CMake where to find `Find*.cmake` files.  
**Why**: We have custom `FindPQXX.cmake` and `FeatureTests.cmake` in `cmake/modules/`.

---

### Creating the Executable
```cmake
add_executable(sqlsmith)
```

**What it does**: Creates a target named "sqlsmith".  
**Why early**: We can add sources/dependencies to it gradually.

```cmake
target_sources(sqlsmith PRIVATE
    src/relmodel.cc
    src/schema.cc
    ...
)
```

**What it does**: Adds source files to compile.  
**PRIVATE**: Only sqlsmith uses these sources (not inherited).

---

### Include Directories (THE CRITICAL PART!)
```cmake
target_include_directories(sqlsmith PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
```

**What it does**: Adds directories to compiler's include search path.

**How it works**:
1. When `src/schema.cc` has `#include "relmodel.hh"`:
2. Compiler searches:
   - Current directory (`src/`) - not found ❌
   - `${CMAKE_CURRENT_SOURCE_DIR}/include` → `include/relmodel.hh` - found! ✅
   - `${CMAKE_CURRENT_BINARY_DIR}` → `build/config.h` - found! ✅

**Variables explained**:
- `CMAKE_CURRENT_SOURCE_DIR` = `/home/darshan/Projects/sqlsmith` (source root)
- `CMAKE_CURRENT_BINARY_DIR` = `/home/darshan/Projects/sqlsmith/build` (build dir)

---

### Finding Dependencies
```cmake
find_package(PostgreSQL REQUIRED)
```

**What it does**:
1. Runs `FindPostgreSQL.cmake` module
2. Searches system for PostgreSQL installation
3. Sets variables: `PostgreSQL_FOUND`, `PostgreSQL_INCLUDE_DIRS`, etc.
4. Creates imported target: `PostgreSQL::PostgreSQL`

**REQUIRED**: CMake errors if not found.

```cmake
target_link_libraries(sqlsmith PRIVATE PostgreSQL::PostgreSQL)
```

**What it does**: Links sqlsmith with PostgreSQL.  
**Modern CMake**: `PostgreSQL::PostgreSQL` is an "imported target" that automatically:
- Adds PostgreSQL include directories
- Links PostgreSQL libraries (libpq)
- Adds required compiler flags

---

### Conditional Logic
```cmake
if (NOT USE_UPSTREAM_LIBPQXX)
    # Use system libpqxx
    find_package(PQXX REQUIRED)
    target_link_libraries(sqlsmith PRIVATE PQXX::PQXX)
else()
    # Download libpqxx from GitHub
    FetchContent_Declare(...)
    FetchContent_MakeAvailable(libpqxx)
    target_link_libraries(sqlsmith PRIVATE pqxx)
endif()
```

**What it does**: Gives user choice how to get libpqxx.  
**Option 1**: Use system-installed version (default).  
**Option 2**: Download from GitHub (if `-DUSE_UPSTREAM_LIBPQXX=ON`).

---

### Generating Files
```cmake
configure_file(config_h.cmake.in config.h)
```

**What it does**: Generates `build/config.h` from template.  
**How**: Replaces `@VARIABLE@` placeholders:
```cpp
// config_h.cmake.in
#define VERSION "@PROJECT_VERSION@"

// Generated build/config.h
#define VERSION "1.4.0"
```

```cmake
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gitrev.h "#define GITREV \"${GIT_TAG}\"\n")
```

**What it does**: Creates `build/gitrev.h` with git version.  
**Why**: Source code can show current version at runtime.

---

## Common CMake Variables

### Directories
| Variable | Meaning | Example |
|----------|---------|---------|
| `CMAKE_CURRENT_SOURCE_DIR` | Directory with current CMakeLists.txt | `/home/user/sqlsmith` |
| `CMAKE_CURRENT_BINARY_DIR` | Build directory | `/home/user/sqlsmith/build` |
| `CMAKE_SOURCE_DIR` | Top-level source dir | `/home/user/sqlsmith` |
| `CMAKE_BINARY_DIR` | Top-level build dir | `/home/user/sqlsmith/build` |

### Project Info
| Variable | Meaning | Example |
|----------|---------|---------|
| `PROJECT_NAME` | Project name | `SQLsmith` |
| `PROJECT_VERSION` | Version from project() | `1.4.0` |
| `PROJECT_SOURCE_DIR` | Project source dir | `/home/user/sqlsmith` |

### Compiler Settings
| Variable | Meaning | Example |
|----------|---------|---------|
| `CMAKE_CXX_STANDARD` | C++ standard version | `17` |
| `CMAKE_CXX_COMPILER` | C++ compiler path | `/usr/bin/g++` |
| `CMAKE_BUILD_TYPE` | Build type | `Debug` or `Release` |

---

## Building the Project

### 1. Configure (Generate Build Files)
```bash
mkdir build          # Create build directory
cd build
cmake ..             # Run CMake (.. = parent directory)
```

**What happens**:
- CMake reads `CMakeLists.txt`
- Finds dependencies (PostgreSQL, libpqxx)
- Generates `Makefile` in `build/`
- Creates `config.h` and `gitrev.h`

### 2. Build (Compile)
```bash
make                 # or: cmake --build .
```

**What happens**:
- Compiles all `.cc` files → `.o` object files
- Links object files → `sqlsmith` executable

### 3. Run
```bash
./sqlsmith --help
```

---

## Advanced: CMake Commands

### Build Types
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..      # Debug build (with symbols, no optimization)
cmake -DCMAKE_BUILD_TYPE=Release ..    # Release build (optimized, no symbols)
```

### Custom Options
```bash
cmake -DUSE_UPSTREAM_LIBPQXX=ON ..     # Use GitHub libpqxx instead of system
```

### Installation
```bash
sudo make install                       # Installs to /usr/local/bin/sqlsmith
```

Or custom install location:
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/sqlsmith ..
make install  # Installs to /opt/sqlsmith/bin/sqlsmith
```

---

## Debugging CMake

### See what CMake is doing
```bash
cmake .. --trace                        # Print every command executed
cmake .. --debug-output                 # Debug CMake itself
```

### See variable values
Add to CMakeLists.txt:
```cmake
message(STATUS "PostgreSQL found: ${PostgreSQL_FOUND}")
message(STATUS "Include dirs: ${PostgreSQL_INCLUDE_DIRS}")
```

### See compiler commands
```bash
make VERBOSE=1                          # Show full g++ commands
```

---

## Summary: Key Takeaways

1. **CMake generates build files** - it doesn't compile directly
2. **Targets are recipes** - `add_executable()`, `add_library()`
3. **Properties configure targets** - `target_sources()`, `target_include_directories()`, `target_link_libraries()`
4. **Use PRIVATE for executables** - they can't be dependencies
5. **Imported targets are powerful** - `PostgreSQL::PostgreSQL` handles everything
6. **Separate source and build** - never build in source directory
7. **Variables need `${}`** - `${PROJECT_NAME}`, not `PROJECT_NAME`

---

## Further Reading

- [Official CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [Modern CMake Guide](https://cliutils.gitlab.io/modern-cmake/)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)
