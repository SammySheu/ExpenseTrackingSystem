/**
 * test_main.cpp
 * Main test runner for C++ Expense Tracker tests
 * 
 * C++ TESTING FEATURE:
 * Google Test provides its own main() function via gtest_main library
 * This file can be used for custom initialization if needed
 * 
 * CONTRAST WITH PYTHON:
 * Python: pytest discovers and runs tests automatically
 * C++: Need explicit test runner with main() function
 */

#include <gtest/gtest.h>
#include <iostream>

using namespace std;

/**
 * C++ FEATURE: Custom main() for test runner
 * Initializes Google Test and runs all tests
 * 
 * If using -lgtest_main, this file is optional
 * We include it to demonstrate C++ test setup
 */
int main(int argc, char **argv) {
    cout << "=================================================\n";
    cout << "  C++ Expense Tracker - Test Suite\n";
    cout << "=================================================\n";
    cout << "Testing C++ features:\n";
    cout << "  - RAII and resource management\n";
    cout << "  - STL containers (vector, map, optional)\n";
    cout << "  - Smart pointers (unique_ptr)\n";
    cout << "  - Exception handling\n";
    cout << "  - Memory management\n";
    cout << "=================================================\n\n";
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    cout << "\n=================================================\n";
    if (result == 0) {
        cout << "  ✓ All tests passed!\n";
    } else {
        cout << "  ✗ Some tests failed\n";
    }
    cout << "=================================================\n";
    
    return result;
}

