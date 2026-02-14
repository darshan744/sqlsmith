# Quick Start - Building SQLsmith

## Option 1: Automated Build Script (Easiest!)

```bash
./build.sh
```

That's it! The script will:
- ✅ Check for vcpkg
- ✅ Configure CMake with vcpkg
- ✅ Install libpqxx automatically
- ✅ Build SQLsmith

**Clean rebuild:**
```bash
./build.sh clean
```

---

## Option 2: Manual Build

### 1. Make sure vcpkg is installed:
```bash
# If not installed:
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
```

### 2. Build:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

### 3. Run:
```bash
./build/sqlsmith --help
```

---

## What's Installed?

Thanks to `vcpkg.json`, vcpkg automatically installs:
- **libpqxx** (7.7.0+) - PostgreSQL C++ client library
- **libpq** - PostgreSQL C client library (dependency of libpqxx)
- All transitive dependencies

No manual installation needed! 🎉

---

## Troubleshooting

### "vcpkg not found"
Install vcpkg:
```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
```

### "CMAKE_TOOLCHAIN_FILE not specified"
You must pass the vcpkg toolchain file:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Clean rebuild
```bash
rm -rf build
./build.sh
```

---

## For More Details

- **vcpkg Guide**: See `docs/VCPKG_GUIDE.md`
- **CMake Guide**: See `docs/CMAKE_GUIDE.md`
