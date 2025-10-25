cmake_minimum_required(VERSION 3.14)
project(ExpenseTrackerTests)

# C++ FEATURE: CMake build system (Python doesn't need this)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Find SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Source files (excluding main.cpp)
set(SOURCES
    database.cpp
    models.cpp
    expense_operations.cpp
    utils.cpp
)

# Test files
set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_database.cpp
    tests/test_models.cpp
    tests/test_utils.cpp
    tests/test_expense_operations.cpp
    tests/test_integration.cpp
    tests/test_functional.cpp
)

# Create test executable
# C++ FEATURE: Explicit linking of object files (vs Python's dynamic imports)
add_executable(run_tests ${SOURCES} ${TEST_SOURCES})

# Link libraries
# C++ FEATURE: Manual library linking (Python handles this automatically)
target_link_libraries(run_tests
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${SQLite3_LIBRARIES}
    pthread
)

# Enable testing
enable_testing()
add_test(NAME ExpenseTrackerTests COMMAND run_tests)

# Add custom target for running tests
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

