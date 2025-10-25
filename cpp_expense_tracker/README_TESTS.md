# C++ Expense Tracker - Test Suite

## Overview

This test suite provides comprehensive testing for the C++ Expense Tracking System using **Google Test (gtest)** framework. The tests show C++-specific features including RAII, STL containers, smart pointers, and memory management.

## Test Structure

```
cpp_expense_tracker/
├── tests/
│   ├── test_main.cpp               # Test runner entry point
│   ├── test_helpers.h              # Test fixtures and helper functions
│   ├── test_database.cpp           # Database layer tests (RAII, connections)
│   ├── test_models.cpp             # Data access layer tests (STL, optional)
│   ├── test_utils.cpp              # Utility function tests (validation, formatting)
│   ├── test_expense_operations.cpp # Business logic tests
│   ├── test_integration.cpp        # Integration tests (module interactions)
│   └── test_functional.cpp         # Functional tests (user stories)
├── Makefile.test                   # Test build configuration
└── CMakeLists.txt                  # CMake configuration
```

## Test Coverage

### 1. Database Tests (`test_database.cpp`)

- **RAII Pattern**: Constructor/destructor resource management
- **Connection Management**: Database connection lifecycle
- **Schema Initialization**: Table and default data creation
- **Smart Pointers**: std::unique_ptr usage
- **Exception Safety**: Resource cleanup on exceptions

**Key C++ Features Tested:**

- RAII (Resource Acquisition Is Initialization)
- Destructor cleanup
- Smart pointers (std::unique_ptr)
- Static methods
- Raw pointer management

### 2. Models Tests (`test_models.cpp`)

- **User Operations**: Create, read, validate users
- **Category Operations**: CRUD operations on categories
- **Expense Operations**: Insert, fetch with filters
- **STL Containers**: std::vector operations
- **Optional Types**: std::optional return values
- **Exception Handling**: EXPECT_THROW testing

**Key C++ Features Tested:**

- STL vectors
- std::optional (C++17)
- Pass by reference (const &)
- Pointer parameters for optional values
- Exception handling

### 3. Utils Tests (`test_utils.cpp`)

- **Validation**: Date, amount, string validation
- **Parsing**: String to date/amount conversion
- **Formatting**: Currency, expense, summary formatting
- **String Utilities**: Trim, toLower operations

**Key C++ Features Tested:**

- String manipulation (std::string)
- Reference parameters for output
- String streams (std::ostringstream)
- iomanip formatting

### 4. Expense Operations Tests (`test_expense_operations.cpp`)

- **Record Expense**: Auto-creation of users/categories
- **View Expenses**: Filtering by date, amount, category, user
- **Calculate Summary**: Aggregation and grouping
- **Nested Containers**: map<string, vector<Expense>>

**Key C++ Features Tested:**

- STL containers (vector, map)
- Struct initialization
- Nested data structures
- Iterator usage

### 5. Integration Tests (`test_integration.cpp`)

- **End-to-End Workflows**: Complete user scenarios
- **Module Interactions**: Cross-module operations
- **Transaction Consistency**: Data integrity
- **Memory Management**: STL container lifetimes
- **RAII in Practice**: Resource cleanup verification

**Key C++ Features Tested:**

- RAII pattern in practice
- Resource cleanup
- Exception safety
- STL container lifetimes

### 6. Functional Tests (`test_functional.cpp`)

- **User Stories**: Real-world usage scenarios
- **Complete Flows**: Record → View → Filter → Summarize
- **Multi-User Scenarios**: Independent user tracking
- **Validation**: Input validation and error handling

**Key C++ Features Tested:**

- Complete application flow
- Object lifetime management
- Exception propagation
- STL usage end-to-end

## Prerequisites

### Install Google Test

**macOS:**

```bash
brew install googletest
```

**Ubuntu/Debian:**

```bash
sudo apt-get install libgtest-dev cmake
```

**Build from Source (if needed):**

```bash
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib
```

### Verify Installation

```bash
# Check if gtest is installed
pkg-config --modversion gtest

# Or check library files
ls /usr/local/lib/libgtest*  # macOS
ls /usr/lib/libgtest*        # Linux
```

## Building Tests

### Using Makefile (Recommended)

```bash
# Build test executable
make -f Makefile.test

# Build and run tests
make -f Makefile.test run-tests

# Run specific test suite
make -f Makefile.test run-database-tests
make -f Makefile.test run-models-tests
make -f Makefile.test run-utils-tests

# Clean test artifacts
make -f Makefile.test clean-tests
```

### Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make

# Run tests
ctest --verbose

# Or run directly
./run_tests
```

## Running Tests

### Run All Tests

```bash
./run_tests
```

### Run Specific Test Suite

```bash
# Database tests only
./run_tests --gtest_filter=DatabaseTest.*

# Models tests only
./run_tests --gtest_filter=ModelsTest.*

# Utils tests only
./run_tests --gtest_filter=UtilsTest.*

# Integration tests only
./run_tests --gtest_filter=IntegrationTest.*

# Functional tests only
./run_tests --gtest_filter=FunctionalTest.*
```

### Run Specific Test

```bash
# Run a single test
./run_tests --gtest_filter=DatabaseTest.DatabaseConstructorOpensConnection

# Run tests matching pattern
./run_tests --gtest_filter=*User*
```

### Verbose Output

```bash
./run_tests --gtest_verbose
```

### List All Tests

```bash
./run_tests --gtest_list_tests
```

## Memory Leak Detection (C++ Specific!)

One of the key differences between C++ and Python testing is memory management. C++ requires explicit memory leak checking.

### Using Valgrind

```bash
# Install valgrind (if not installed)
# macOS: brew install valgrind
# Linux: sudo apt-get install valgrind

# Run tests with memory leak detection
make -f Makefile.test valgrind

# Or run directly
valgrind --leak-check=full --show-leak-kinds=all ./run_tests
```

### Expected Output

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: X allocs, X frees, Y bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

## Test Isolation

Each test uses a **temporary database** to ensure isolation:

- Database files: `test_expenses_<timestamp>_<random>.db`
- Created in `SetUp()` method
- Deleted in `TearDown()` method
- **Main `expenses.db` is never touched by tests**

### Verify Isolation

```bash
# Check that main database is not affected
ls -la expenses.db

# Run tests
./run_tests

# Main database should be unchanged
ls -la expenses.db
```

## Test Results

### Success Output

```
=================================================
  C++ Expense Tracker - Test Suite
=================================================
Testing C++ features:
  - RAII and resource management
  - STL containers (vector, map, optional)
  - Smart pointers (unique_ptr)
  - Exception handling
  - Memory management
=================================================

[==========] Running X tests from Y test suites.
[----------] Global test environment set-up.
...
[----------] Global test environment tear-down
[==========] X tests from Y test suites ran. (Z ms total)
[  PASSED  ] X tests.

=================================================
  ✓ All tests passed!
=================================================
```

### Failure Output

```
[ RUN      ] TestSuite.TestName
path/to/test.cpp:123: Failure
Expected: (value) == (expected), actual: 10 vs 20
[  FAILED  ] TestSuite.TestName (1 ms)
```

## C++ vs Python Testing Comparison

| Aspect             | C++ (Google Test)   | Python (pytest)  |
| ------------------ | ------------------- | ---------------- |
| **Framework**      | Google Test         | pytest           |
| **Compilation**    | Required            | Not required     |
| **Memory Testing** | Valgrind            | Not needed (GC)  |
| **Fixtures**       | SetUp/TearDown      | @pytest.fixture  |
| **Assertions**     | EXPECT*\*/ASSERT*\* | assert           |
| **Mocking**        | Google Mock         | pytest-mock      |
| **Containers**     | STL (vector, map)   | list, dict       |
| **Optional**       | std::optional       | None             |
| **Exceptions**     | EXPECT_THROW        | pytest.raises    |
| **RAII**           | Tested explicitly   | N/A (GC handles) |

## Key C++ Testing Concepts

### 1. RAII Testing

```cpp
TEST(DatabaseTest, RAIICleanup) {
    {
        Database db("test.db");
        // Use database
    } // Destructor called here
    // Verify cleanup happened
}
```

### 2. Smart Pointer Testing

```cpp
TEST(MemoryTest, SmartPointerCleanup) {
    auto db = std::make_unique<Database>("test.db");
    EXPECT_NE(db, nullptr);
    // No manual delete needed
}
```

### 3. Exception Testing

```cpp
TEST(ModelsTest, ThrowsOnInvalidInput) {
    EXPECT_THROW(
        Models::createUser(db, ""),
        std::invalid_argument
    );
}
```

### 4. Optional Testing

```cpp
TEST(ModelsTest, ReturnsOptional) {
    auto user = Models::getUserByName(db, "NonExistent");
    EXPECT_FALSE(user.has_value());
}
```

### 5. Vector Testing

```cpp
TEST(ModelsTest, ReturnsVector) {
    std::vector<User> users = Models::getAllUsers(db);
    EXPECT_EQ(users.size(), 0);
    EXPECT_TRUE(users.empty());
}
```

## Troubleshooting

### Google Test Not Found

```bash
# Install Google Test
brew install googletest  # macOS
sudo apt-get install libgtest-dev  # Linux
```

### Linker Errors

```bash
# Check library paths
echo $LIBRARY_PATH
echo $LD_LIBRARY_PATH

# Add to Makefile.test if needed:
LDFLAGS += -L/usr/local/lib
```

### SQLite Not Found

```bash
# Install SQLite development files
brew install sqlite  # macOS
sudo apt-get install libsqlite3-dev  # Linux
```

### Compilation Errors

```bash
# Ensure C++17 support
g++ --version  # Should be 7.0+

# Check compiler flags
make -f Makefile.test CXXFLAGS="-std=c++17 -v"
```

## Test Statistics

- **Total Tests**: 100+
- **Test Files**: 7
- **Test Suites**: 6
- **Lines of Test Code**: ~2000+
- **Coverage**: All modules tested

## Expected Test Execution Time

- **All Tests**: < 10 seconds
- **Database Tests**: < 2 seconds
- **Models Tests**: < 3 seconds
- **Utils Tests**: < 1 second
- **Operations Tests**: < 2 seconds
- **Integration Tests**: < 3 seconds
- **Functional Tests**: < 2 seconds

## Contributing

When adding new features:

1. Write tests first (TDD approach)
2. Ensure all existing tests pass
3. Add tests for new functionality
4. Run valgrind to check for memory leaks
5. Update this README if needed

## License

Part of the C++ Expense Tracking System project.
