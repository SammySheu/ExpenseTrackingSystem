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

/**
 * C++ FEATURE: Custom main() for test runner
 * Initializes Google Test and runs all tests
 * 
 * If using -lgtest_main, this file is optional
 * We include it to demonstrate C++ test setup
 */
int main(int argc, char **argv) {
    std::cout << "=================================================\n";
    std::cout << "  C++ Expense Tracker - Test Suite\n";
    std::cout << "=================================================\n";
    std::cout << "Testing C++ features:\n";
    std::cout << "  - RAII and resource management\n";
    std::cout << "  - STL containers (vector, map, optional)\n";
    std::cout << "  - Smart pointers (unique_ptr)\n";
    std::cout << "  - Exception handling\n";
    std::cout << "  - Memory management\n";
    std::cout << "=================================================\n\n";
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    std::cout << "\n=================================================\n";
    if (result == 0) {
        std::cout << "  ✓ All tests passed!\n";
    } else {
        std::cout << "  ✗ Some tests failed\n";
    }
    std::cout << "=================================================\n";
    
    return result;
}

