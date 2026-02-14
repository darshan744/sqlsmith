# 🚀 Building SQLsmith with vcpkg

This guide explains how to build SQLsmith using vcpkg for dependency management.

## What is vcpkg?

vcpkg is a C++ package manager that:
- ✅ Downloads and compiles dependencies automatically
- ✅ Works across Windows, Linux, macOS
- ✅ Integrates seamlessly with CMake
- ✅ Handles transitive dependencies (libpqxx → libpq → PostgreSQL)

---

## Prerequisites

### 1. Install vcpkg (if not already installed)

```bash
# Clone vcpkg
cd ~
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
./bootstrap-vcpkg.sh

# Add to PATH (optional, recommended)
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

---

## Building SQLsmith

### Method 1: Manifest Mode (Recommended - Automatic!)

With `vcpkg.json` in your project, vcpkg automatically installs dependencies:

```bash
cd /home/darshan/Projects/sqlsmith

# Configure with vcpkg toolchain
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake

# vcpkg will automatically install libpqxx during configure!

# Build
cmake --build build

# Run
./build/sqlsmith --help
```

### Method 2: Manual Installation First

If you prefer to install dependencies manually:

```bash
# Install libpqxx via vcpkg
~/vcpkg/vcpkg install libpqxx

# Then configure your project
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build
```

---

## Understanding vcpkg.json (Our Manifest)

```json
{
  "name": "sqlsmith",
  "version": "1.4.0",
  "dependencies": [
    {
      "name": "libpqxx",
      "version>=": "7.7.0"
    }
  ]
}
```

**What this does:**
- Declares `libpqxx` as a dependency
- Requires version 7.7.0 or newer
- vcpkg automatically installs libpqxx AND its dependencies:
  - libpqxx → libpq → PostgreSQL libraries

---

## How vcpkg Works with CMake

### 1. Without vcpkg:
```cmake
find_package(libpqxx CONFIG REQUIRED)  # ❌ Error: libpqxx not found!
```

### 2. With vcpkg:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
```

**What happens:**
- vcpkg toolchain tells CMake where to find packages
- `find_package(libpqxx)` → looks in vcpkg's installed packages
- CMake finds `libpqxxConfig.cmake` from vcpkg
- Sets up include paths and libraries automatically

---

## Dependency Chain Explained

When you install libpqxx via vcpkg:

```
vcpkg install libpqxx
    ↓ depends on
vcpkg install libpq-dev (PostgreSQL client library)
    ↓ depends on
vcpkg install openssl, zlib (PostgreSQL dependencies)
```

**All automatic!** vcpkg handles the entire dependency tree.

---

## Troubleshooting

### Error: "CMAKE_TOOLCHAIN_FILE not specified"

**Solution:** You forgot to pass the vcpkg toolchain file:
```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Error: "libpqxx not found"

**Solution 1:** Clean and reconfigure:
```bash
rm -rf build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Solution 2:** Manually install first:
```bash
~/vcpkg/vcpkg install libpqxx
```

### Error: "PostgreSQL not found"

vcpkg should install PostgreSQL libraries automatically. If not:
```bash
~/vcpkg/vcpkg install libpq
```

---

## vcpkg vs System Packages

### System packages (apt/dnf):
```bash
sudo apt install libpqxx-dev  # Installs system-wide
```

**Pros:** Simple, one command  
**Cons:** May be outdated, conflicts with other projects

### vcpkg:
```bash
vcpkg install libpqxx  # Installs to vcpkg directory
```

**Pros:** 
- ✅ Latest versions
- ✅ Isolated per-project
- ✅ Cross-platform
- ✅ Easy to update: `vcpkg upgrade`

**Cons:**
- Takes longer (compiles from source)
- Larger disk usage

---

## Advanced vcpkg Usage

### Pin specific versions:
```json
{
  "dependencies": [
    {
      "name": "libpqxx",
      "version>=": "7.7.5"
    }
  ],
  "overrides": [
    {
      "name": "libpqxx",
      "version": "7.7.5"
    }
  ]
}
```

### Add more dependencies:
```json
{
  "dependencies": [
    "libpqxx",
    "boost-regex",
    "fmt"
  ]
}
```

### Check installed packages:
```bash
~/vcpkg/vcpkg list
```

### Update packages:
```bash
cd ~/vcpkg
git pull
./bootstrap-vcpkg.sh
~/vcpkg/vcpkg upgrade --no-dry-run
```

---

## Quick Reference

### Essential Commands:

```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh

# Build SQLsmith with vcpkg
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build

# Clean rebuild
rm -rf build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build

# Check installed packages
~/vcpkg/vcpkg list
```

---

## Why This Approach is Better

### Before (Manual dependency hunting):
1. ❌ Find PostgreSQL dev packages
2. ❌ Find libpqxx (may not exist in repos)
3. ❌ Check version compatibility
4. ❌ Install system-wide (conflicts possible)
5. ❌ Different process on each OS

### After (with vcpkg):
1. ✅ One command: `vcpkg install libpqxx`
2. ✅ Or automatic with vcpkg.json manifest mode
3. ✅ Always latest compatible versions
4. ✅ Isolated, no system pollution
5. ✅ Same process on Windows, Linux, macOS

---

## Summary

**vcpkg + CMake = Easy C++ Dependency Management** 🎉

Your project now uses **manifest mode** - just run cmake with the toolchain file and vcpkg handles everything automatically!

```bash
# That's it! Just this one command:
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

vcpkg will:
- Read `vcpkg.json`
- Install libpqxx (and its dependencies)
- Configure CMake to find everything
- You just compile and run!
