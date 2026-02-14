# SQLsmith

![SQLsmith Logo](docs/logo.png)

> "I love the smell of coredumps in the morning" - mba

A random SQL query generator for finding bugs in PostgreSQL through fuzz testing.

## About This Fork

This is a modernized fork of [SQLsmith](https://github.com/anse1/sqlsmith) focusing exclusively on **PostgreSQL support** with a streamlined, modern C++ codebase.

**Key Changes:**
- 🎯 **PostgreSQL-only** - Removed SQLite and MonetDB support for simplicity
- 🏗️ **Modern build system** - CMake-only, vcpkg dependency management
- 📁 **Clean structure** - Organized `src/` and `include/` directories
- 🎨 **Code standards** - Google C++ Style Guide, `.clang-format`, `.editorconfig`
- 🚀 **C++17** - Updated to modern C++ standard

## What is SQLsmith?

SQLsmith is a random SQL query generator inspired by [Csmith](https://embed.cs.utah.edu/csmith/). It generates complex, syntactically valid SQL queries to stress-test PostgreSQL and uncover bugs, crashes, and edge cases.

**Since 2015, the original SQLsmith has discovered 118+ bugs** in production database systems, including critical security vulnerabilities. This fork continues that mission with a focus on PostgreSQL.

## Features

- 🎲 **Random SQL Generation** - Creates complex, valid SQL queries automatically
- 🐘 **PostgreSQL Support** - Full support for PostgreSQL testing
- 🐛 **Bug Discovery** - Proven track record of finding critical database bugs
- 📈 **Large-scale Testing** - Supports centralized logging for multiple instances
- 🎯 **Smart Blacklisting** - Automatically learns and avoids problematic query patterns

## Quick Start

### Prerequisites

- **C++17** compiler (GCC 7+, Clang 5+)
- **CMake 3.14+**
- **vcpkg** (for dependency management)
- **PostgreSQL** (database to test)

### Building with vcpkg

```bash
# Clone the repository
git clone https://github.com/darshan744/sqlsmith.git
cd sqlsmith

# Configure with vcpkg (vcpkg will auto-install libpqxx)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build -j$(nproc)

# Run
./build/sqlsmith --help
```

### If You Don't Have vcpkg

```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh

# Then build as above
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build -j$(nproc)
```

## Usage

### Basic Testing

SQLsmith connects to PostgreSQL to:
1. Retrieve the database schema
2. Generate random SQL queries based on that schema
3. Execute queries (in rolled-back transactions)

**⚠️ Important:** Use an **unprivileged database user** for testing, as SQLsmith may call functions with side effects (e.g., `pg_terminate_backend()`).

```bash
# Test a local PostgreSQL database
./build/sqlsmith --verbose --target="host=localhost port=5432 dbname=testdb user=testuser"

# Dry run (print queries without executing)
./build/sqlsmith --dry-run --target="dbname=testdb" --max-queries=100

# Run with specific seed for reproducibility
./build/sqlsmith --verbose --seed=12345 --target="dbname=testdb"
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--target=connstr` | PostgreSQL connection string (default: libpq defaults) |
| `--log-to=connstr` | PostgreSQL database for centralized error logging |
| `--verbose` | Enable progress output to stderr |
| `--version` | Show version information |
| `--seed=int` | Seed RNG with specified integer (default: PID) |
| `--dry-run` | Print queries without executing them |
| `--max-queries=N` | Terminate after generating N queries |
| `--exclude-catalog` | Don't use catalog relations in queries |
| `--dump-all-queries` | Print all generated queries |
| `--dump-all-graphs` | Dump generated ASTs for debugging |
| `--rng-state=string` | Deserialize a previously saved RNG state |

### Understanding Output

When using `--verbose`, SQLsmith outputs progress symbols:

| Symbol | Meaning | Description |
|--------|---------|-------------|
| `.` | Success | Query executed successfully |
| `S` | Syntax Error | **Bug in SQLsmith** - please report! |
| `t` | Timeout | Query exceeded 1-second timeout |
| `C` | Connection Broken | **Query crashed the database server** |
| `e` | Other Error | Various SQL errors (expected) |

### Sample Output

```
queries: 39000 (202.399 gen/s, 298.942 exec/s)
AST stats (avg): height = 5.599 nodes = 37.8489
82      ERROR:  invalid regular expression: quantifier operand invalid
70      ERROR:  canceling statement due to statement timeout
44      ERROR:  operator does not exist: point = point
...
error rate: 0.00705128
```

## Advanced Usage

### Large-Scale Testing

SQLsmith supports logging from multiple concurrent instances to a central PostgreSQL database:

1. Set up the logging database using the schema in `tools/log.sql`
2. Use the `--log-to` option to specify the logging database

```bash
sqlsmith --verbose --target="dbname=test" --log-to="dbname=sqlsmith_logs"
```

### Adaptive Learning

SQLsmith automatically blacklists grammar productions that consistently lead to errors. When testing begins, you'll see syntax errors as SQLsmith learns which SQL patterns your PostgreSQL version supports. These errors will decrease over time.

## Project Structure

```
sqlsmith/
├── src/              # Source files (.cc)
├── include/          # Header files (.hh)
├── cmake/            # CMake modules
│   └── modules/      # Custom Find*.cmake scripts
├── tools/            # Utility scripts and SQL schemas
├── docs/             # Documentation
├── CMakeLists.txt    # Build configuration
└── vcpkg.json        # Dependency manifest (if added)
```

## Development

### Code Style

This project follows the **Google C++ Style Guide** with 4-space indentation.

- Format code: `clang-format -i src/*.cc include/*.hh`
- EditorConfig and .clang-format are provided for automatic formatting

### Building for Development

```bash
# Enable compile_commands.json for IDE/LSP support (already enabled by default)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build

# Clean rebuild
rm -rf build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Testing Best Practices

1. **Use a dedicated test database** - Never run SQLsmith on production systems
2. **Use unprivileged users** - Limit permissions to prevent destructive actions
3. **Monitor server resources** - SQLsmith can generate resource-intensive queries
4. **Review crashes carefully** - Server crashes (symbol `C`) are critical bugs
5. **Save interesting queries** - Use `--dump-all-queries` or logging

## Upstream & Attribution

This fork is based on the original [SQLsmith](https://github.com/anse1/sqlsmith) project.

**Original Authors:**
- Andreas Seltenreich - <seltenreich@gmx.de>
- Bo Tang - <tangloner@gmail.com>
- Sjoerd Mullender - <sjoerd@acm.org>

**This Fork:**
- Maintained by: [darshan744](https://github.com/darshan744)
- Repository: https://github.com/darshan744/sqlsmith

## Resources

- [Original SQLsmith Wiki](https://github.com/anse1/sqlsmith/wiki)
- [Bug Score List](https://github.com/anse1/sqlsmith/wiki#score-list)
- [PGConf.EU 2018 Slides](https://www.postgresql.eu/events/pgconfeu2018/sessions/session/2221/slides/145/sqlsmith-talk.pdf)
- [Doxygen Documentation](https://anse1.github.io/sqlsmith-doc/structsqltype.html)

## Contributing

Contributions are welcome! Whether you're fixing bugs, adding features, or improving documentation, feel free to open issues or pull requests.

**Found a bug in SQLsmith?** If SQLsmith generates syntactically invalid queries (symbol `S`), please report it!

## License

SQLsmith is licensed under **GPLv3**. See [COPYING](COPYING) for full details.

**⚠️ WARNING:** SQLsmith is designed to stress-test databases and may trigger crashes or data corruption. Use at your own risk on non-production systems only. The tool is intended to find bugs by intentionally pushing databases to their limits.

## Acknowledgments

- Original SQLsmith project and authors
- [Csmith](https://embed.cs.utah.edu/csmith/) - The inspiration for SQLsmith
- PostgreSQL community for supporting testing efforts

---

<p align="center">
  <strong>Built with ❤️ for PostgreSQL quality assurance</strong><br>
  If SQLsmith helped you find a bug, we'd love to hear about it!
</p>
