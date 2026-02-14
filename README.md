# SQLsmith[[logo.png]]

* SQLsmith

> "I love the smell of coredumps in the morning" - mba

: <mba> "I love the smell of coredumps in the morning"

A random SQL query generator for finding bugs in database management systems through fuzz testing.

** Description

![SQLsmith Logo](logo.png)SQLsmith is a random SQL query generator.  Its paragon is [[https://embed.cs.utah.edu/csmith/][Csmith]],

which proved valuable for quality assurance in C compilers.

## About

It currently supports generating queries for PostgreSQL, SQLite 3 and

SQLsmith is a random SQL query generator inspired by [Csmith](https://embed.cs.utah.edu/csmith/), which has proven invaluable for quality assurance in C compilers. It generates complex, syntactically valid SQL queries to stress-test database systems and uncover bugs, crashes, and edge cases.MonetDB.  To add support for another RDBMS, you need to implement two

classes providing schema information about and connectivity to the

**Since 2015, SQLsmith has discovered 118+ bugs** in production database systems, including critical security vulnerabilities in released versions. Additional bugs have been found in database extensions and libraries such as orafce and glibc.device under test.



## FeaturesBesides developers of the RDBMS products, users developing extensions

might also be interested in exposing their code to SQLsmith's random

- 🎲 **Random SQL Generation** - Creates complex, valid SQL queries automaticallyworkload.

- 🐘 **PostgreSQL Support** - Full support for PostgreSQL testing

- 🗄️ **SQLite3 Support** - Test SQLite databasesSince 2015, it found 118 bugs in alphas, betas and releases in the

- 📊 **MonetDB Support** - MonetDB compatibilityaforementioned products, including security vulnerabilities in

- 🐛 **Proven Bug Hunter** - 118+ bugs found in production systemsreleased versions.  Additional bugs were squashed in extensions and

- 🔒 **Security Testing** - Discovered vulnerabilities in released versionslibraries such as orafce and glibc.

- 📈 **Large-scale Testing** - Supports centralized logging for multiple instances

- 🎯 **Smart Blacklisting** - Automatically learns and avoids problematic query patterns    https://github.com/anse1/sqlsmith/wiki#score-list



## Quick Start** Dependencies

- C++11

### Prerequisites- libpqxx



**Required:**optional:

- C++17 or later- boost::regex in case your std::regex is broken

- CMake 3.14+- SQLite3

- libpqxx (PostgreSQL C++ client library)- monetdb_mapi



**Optional:**** Building on Debian

- boost::regex (if std::regex is not available)

- SQLite3 development libraries: apt-get install build-essential autoconf autoconf-archive libpqxx-dev libboost-regex-dev libsqlite3-dev

- MonetDB mapi client library: cd sqlsmith

: autoreconf -i # Avoid when building from a release tarball

### Building: ./configure

: make

#### Debian/Ubuntu

** Building on OSX

```bash

# Install dependenciesIn order to build on Mac OSX, assuming you use Homebrew, run the following

sudo apt-get install build-essential cmake libpqxx-dev libboost-regex-dev libsqlite3-dev

: brew install libpqxx automake libtool autoconf autoconf-archive pkg-config

# Build SQLsmith: cd sqlsmith

mkdir build && cd build: autoreconf -i # Avoid when building from a release tarball

cmake ..: ./configure

make: make

```

** Usage

#### macOS (with Homebrew)

SQLsmith connects to the target database to retrieve the schema for

```bashquery generation and to send the generated queries to.  Currently, all

# Install dependenciesgenerated statements are rolled back.  Beware that SQLsmith does call

brew install cmake libpqxx boost sqlite3functions that could possibly have side-effects

(e.g. pg_terminate_backend).  Use a suitably *underprivileged user*

# Build SQLsmithfor its connection to avoid this.

mkdir build && cd build

cmake ..Example invocations:

make

```: # testing Postgres

: sqlsmith --verbose --target="host=/tmp port=65432 dbname=regression"

### Installation: # testing SQLite

: sqlsmith --verbose --sqlite="file:$HOME/.mozilla/firefox/places.sqlite?mode=ro"

```bash: # testing MonetDB

# From the build directory: sqlsmith --verbose --monetdb="mapi:monetdb://localhost:50000/smith"

sudo make install

```The following options are currently supported:



## Usage| =--target=connstr=   | target postgres database (default: libpq defaults)       |

| =--sqlite=URI=       | target SQLite3 database                                  |

### Basic Usage| =--monetdb=URI=      | target MonetDB database                                  |

| =--log-to=connstr=   | postgres db for logging errors into (default: don't log) |

SQLsmith connects to a target database to:| =--verbose=          | emit progress output                                     |

1. Retrieve the database schema| =--version=          | show version information                                 |

2. Generate random SQL queries based on that schema| =--seed=int=         | seed RNG with specified integer instead of PID           |

3. Execute queries (in rolled-back transactions)| =--dry-run=          | print queries instead of executing them                  |

| =--max-queries=long= | terminate after generating this many queries             |

**⚠️ Important:** Use an **unprivileged database user** for testing, as SQLsmith may call functions with side effects (e.g., `pg_terminate_backend()`).| =--exclude-catalog=  | don't generate queries using catalog relations           |

| =--dump-all-queries= | dump queries as they are generated                       |

### Example Invocations| =--dump-all-graphs=  | dump generated ASTs for debugging                        |

| =--rng-state=string= | deserialize dumped rng state                             |

**Testing PostgreSQL:**

```bashSample output:

sqlsmith --verbose --target="host=localhost port=5432 dbname=testdb user=testuser"

```=--verbose= makes sqlsmith emit some progress indication to stderr.  A

symbol is output for each query sent to the server.  Currently the

**Testing SQLite:**following ones are generated:

```bash

sqlsmith --verbose --sqlite="file:./test.db?mode=ro"| symbol | meaning           | details                                       |

```|--------+-------------------+-----------------------------------------------|

| .      | ok                | Query generated and executed with ok sqlstate |

**Testing MonetDB:**| S      | syntax error      | These are bugs in sqlsmith - please report    |

```bash| t      | timeout           | SQLsmith sets a statement timeout of 1s       |

sqlsmith --verbose --monetdb="mapi:monetdb://localhost:50000/testdb"| C      | broken connection | These happen when a query crashes the server  |

```| e      | other error       |                                               |



**Dry Run (print queries without executing):**When you test against a RDBMS that doesn't support some of SQLsmith's

```bashgrammar, there will be a burst of syntax errors on startup.  These

sqlsmith --dry-run --target="dbname=testdb" --max-queries=100should disappear after some time as SQLsmith blacklists productions

```that consistently lead to errors.



### Command-Line Options=--verbose= will also periodically emit error reports.  In the

following example, these are mostly caused by the primitive type

| Option | Description |system.

|--------|-------------|

| `--target=connstr` | PostgreSQL connection string (default: libpq defaults) |: queries: 39000 (202.399 gen/s, 298.942 exec/s)

| `--sqlite=URI` | SQLite3 database URI |: AST stats (avg): height = 5.599 nodes = 37.8489

| `--monetdb=URI` | MonetDB database connection URI |: 82	ERROR:  invalid regular expression: quantifier operand invalid

| `--log-to=connstr` | PostgreSQL database for centralized error logging |: 70	ERROR:  canceling statement due to statement timeout

| `--verbose` | Enable progress output to stderr |: 44	ERROR:  operator does not exist: point = point

| `--version` | Show version information |: 27	ERROR:  operator does not exist: xml = xml

| `--seed=int` | Seed RNG with specified integer (default: PID) |: 22	ERROR:  cannot compare arrays of different element types

| `--dry-run` | Print queries without executing them |: 11	ERROR:  could not determine which collation to use for string comparison

| `--max-queries=N` | Terminate after generating N queries |: 5	ERROR:  invalid regular expression: nfa has too many states

| `--exclude-catalog` | Don't use catalog relations in queries |: 4	ERROR:  cache lookup failed for index 2619

| `--dump-all-queries` | Print all generated queries |: 4	ERROR:  invalid regular expression: brackets [] not balanced

| `--dump-all-graphs` | Dump generated ASTs for debugging |: 3	ERROR:  operator does not exist: polygon = polygon

| `--rng-state=string` | Deserialize a previously saved RNG state |: 2	ERROR:  invalid regular expression: parentheses () not balanced

: 1	ERROR:  invalid regular expression: invalid character range

### Understanding Output: error rate: 0.00705128



When using `--verbose`, SQLsmith outputs progress symbols:The only one that looks interesting here is the cache lookup one.

Taking a closer look at it reveals that it happens when you query a

| Symbol | Meaning | Description |certain catalog view like this:

|--------|---------|-------------|

| `.` | Success | Query executed successfully |: self=# select indexdef from pg_catalog.pg_indexes where indexdef is not NULL;

| `S` | Syntax Error | **Bug in SQLsmith** - please report! |: FEHLER:  cache lookup failed for index 2619

| `t` | Timeout | Query exceeded 1-second timeout |

| `C` | Connection Broken | Query crashed the database server |This is because the planner then puts =pg_get_indexdef(oid)= in a

| `e` | Other Error | Various SQL errors (expected) |context where it sees non-index-oids, which causes it to croak:



### Sample Output:                                      QUERY PLAN                                     

: ------------------------------------------------------------------------------------

```:  Hash Join  (cost=17.60..30.65 rows=9 width=4)

queries: 39000 (202.399 gen/s, 298.942 exec/s):    Hash Cond: (i.oid = x.indexrelid)

AST stats (avg): height = 5.599 nodes = 37.8489:    ->  Seq Scan on pg_class i  (cost=0.00..12.52 rows=114 width=8)

82      ERROR:  invalid regular expression: quantifier operand invalid:          Filter: ((pg_get_indexdef(oid) IS NOT NULL) AND (relkind = 'i'::"char"))

70      ERROR:  canceling statement due to statement timeout:    ->  Hash  (cost=17.31..17.31 rows=23 width=4)

44      ERROR:  operator does not exist: point = point:          ->  Hash Join  (cost=12.52..17.31 rows=23 width=4)

27      ERROR:  operator does not exist: xml = xml:                Hash Cond: (x.indrelid = c.oid)

22      ERROR:  cannot compare arrays of different element types:                ->  Seq Scan on pg_index x  (cost=0.00..4.13 rows=113 width=8)

...:                ->  Hash  (cost=11.76..11.76 rows=61 width=8)

error rate: 0.00705128:                      ->  Seq Scan on pg_class c  (cost=0.00..11.76 rows=61 width=8)

```:                            Filter: (relkind = ANY ('{r,m}'::"char"[]))



## Advanced UsageNow this is more of a curiosity than a bug, but still illustrating how

debugging with the help of SQLsmith might look like.

### Large-Scale Testing

** Large-scale testing

SQLsmith supports logging from multiple concurrent instances to a central PostgreSQL database for coordinated testing campaigns.

=--log-to= allows logging of hundreds of sqlsmith instances into a

1. Set up the logging database using the schema in `log.sql`central PostgreSQL database. [[./log.sql]] contains the schema sqlsmith

2. Use the `--log-to` option to specify the logging databaseexpects and some additional views to generate reports on the logged

3. Use filter tables (`known.txt`, `known_re.txt`) to exclude known errorscontents.



```bashIt also contains a trigger to filter boring/known errors based on the

sqlsmith --verbose --target="dbname=test" --log-to="dbname=sqlsmith_logs"contents of the tables known and known_re.  I periodically COPY my

```filter tables for testing PostgreSQL into the files [[./known_re.txt]] and

[[./known.txt]] to serve as a starting point.

### Adaptive Learning

** Resources

SQLsmith automatically blacklists grammar productions that consistently lead to errors. When testing a database that doesn't support certain SQL features, you'll initially see syntax errors, but these will decrease as SQLsmith learns which patterns to avoid.

- [[https://www.postgresql.eu/events/pgconfeu2018/sessions/session/2221/slides/145/sqlsmith-talk.pdf][Slides from PGConf.EU 2018]]

## Architecture- [[https://anse1.github.io/sqlsmith-doc/structsqltype.html][Doxygen output for SQLsmith]]



SQLsmith uses a grammar-based approach:** License



1. **Schema Introspection** - Loads tables, columns, types, operators, and functions from the target databaseSQLsmith is available under GPLv3.  Use it at your own risk.  It may

2. **Random AST Generation** - Builds abstract syntax trees for SQL queries*damage your database* (one of the purposes of this tool /is/ to try

3. **Query Generation** - Converts ASTs to SQL textand break things).  See the file [[COPYING]] for details.

4. **Execution & Feedback** - Executes queries and learns from failures

** Authors

## Extending SQLsmith

Andreas Seltenreich <seltenreich@gmx.de>

To add support for a new RDBMS, implement two classes:

Bo Tang <tangloner@gmail.com>

1. **Schema Class** - Provides schema information (tables, types, operators, functions)

2. **DUT Class** (Device Under Test) - Handles database connectivity and query executionSjoerd Mullender <sjoerd@acm.org>



See `postgres.hh`/`postgres.cc`, `sqlite.hh`/`sqlite.cc`, or `monetdb.hh`/`monetdb.cc` for examples.[[ast.png]]


## Project Structure

```
sqlsmith/
├── src/              # Source files (.cc)
├── include/          # Header files (.hh)
├── cmake/            # CMake modules
├── debian/           # Debian packaging
└── CMakeLists.txt    # Build configuration
```

## Documentation

- [Wiki](https://github.com/anse1/sqlsmith/wiki)
- [Bug Score List](https://github.com/anse1/sqlsmith/wiki#score-list)
- [PGConf.EU 2018 Slides](https://www.postgresql.eu/events/pgconfeu2018/sessions/session/2221/slides/145/sqlsmith-talk.pdf)
- [Doxygen Documentation](https://anse1.github.io/sqlsmith-doc/structsqltype.html)

## Contributing

Contributions are welcome! Whether you're fixing bugs, adding features, or improving documentation, we appreciate your help.

**Found a bug in SQLsmith?** If SQLsmith generates syntactically invalid queries (symbol `S`), please report it as a bug!

## Testing Best Practices

1. **Use a dedicated test database** - Never run SQLsmith on production systems
2. **Use unprivileged users** - Limit permissions to prevent destructive actions
3. **Monitor server resources** - SQLsmith can generate resource-intensive queries
4. **Review crashes carefully** - Server crashes (symbol `C`) are critical bugs in the database
5. **Save interesting queries** - Use `--dump-all-queries` or logging to capture problematic queries

## License

SQLsmith is licensed under **GPLv3**. See [COPYING](COPYING) for full details.

**⚠️ WARNING:** SQLsmith is designed to stress-test databases and may trigger crashes or data corruption. Use at your own risk on non-production systems only. The tool is intended to find bugs by intentionally pushing databases to their limits.

## Authors

- **Andreas Seltenreich** - <seltenreich@gmx.de>
- **Bo Tang** - <tangloner@gmail.com>
- **Sjoerd Mullender** - <sjoerd@acm.org>

## Acknowledgments

SQLsmith is inspired by [Csmith](https://embed.cs.utah.edu/csmith/), the highly successful random C program generator used for compiler testing.

---

<p align="center">
  <strong>Built with ❤️ for database quality assurance</strong><br>
  If SQLsmith helped you find a bug, we'd love to hear about it!
</p>
