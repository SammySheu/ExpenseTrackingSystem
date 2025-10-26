/**
 * test_helpers.h
 * Test fixtures and helper functions for C++ Expense Tracker tests
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - Google Test fixtures with SetUp/TearDown
 * - Smart pointers for RAII in tests
 * - Test inheritance for code reuse
 * - Helper functions for test data creation
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>
#include "../database.h"
#include "../models.h"
#include "../expense.h"
#include "../expense_operations.h"

/**
 * C++ FEATURE: Test Fixture with RAII
 * Base test class that provides a clean database for each test
 * SetUp() runs before each test, TearDown() runs after
 * 
 * CONTRAST WITH PYTHON:
 * Python uses @pytest.fixture decorators
 * C++ uses class inheritance with virtual methods
 */
class DatabaseTest : public ::testing::Test {
protected:
    // C++ FEATURE: Smart pointer for automatic cleanup
    std::unique_ptr<Database> db;
    std::string test_db_path;
    
    /**
     * SetUp runs before each test
     * C++ FEATURE: Virtual method override
     */
    void SetUp() override {
        // Create unique test database path with timestamp
        test_db_path = "test_expenses_" + 
                       std::to_string(std::time(nullptr)) + "_" +
                       std::to_string(rand()) + ".db";
        
        // C++ FEATURE: std::make_unique for smart pointer creation
        db = std::make_unique<Database>(test_db_path);
        db->initialize();
    }
    
    /**
     * TearDown runs after each test
     * C++ FEATURE: Automatic cleanup with RAII
     */
    void TearDown() override {
        // Reset smart pointer (calls Database destructor)
        db.reset();
        
        // Remove test database file
        std::remove(test_db_path.c_str());
    }
};

/**
 * C++ FEATURE: Test fixture inheritance
 * ModelsTest inherits from DatabaseTest to reuse database setup
 * 
 * CONTRAST WITH PYTHON:
 * Python uses fixture dependencies (@pytest.fixture)
 * C++ uses class inheritance
 */
class ModelsTest : public DatabaseTest {
protected:
    /**
     * Helper: Create a sample user for testing
     * Returns user ID
     */
    int createSampleUser(const std::string& name = "TestUser") {
        return Models::createUser(*db, name);
    }
    
    /**
     * Helper: Get first default category
     * Returns category ID
     */
    int createSampleCategory() {
        auto categories = Models::getAllCategories(*db);
        return categories.empty() ? 0 : categories[0].id;
    }
    
    /**
     * Helper: Create a sample expense
     * Returns expense ID
     */
    int createSampleExpense(int user_id, int category_id) {
        return Models::insertExpense(*db, "2025-10-25", category_id, 
                                    "Test Expense", 50.0, user_id);
    }
};

/**
 * C++ FEATURE: Helper function for creating test data
 * Creates multiple expenses for testing
 * 
 * CONTRAST WITH PYTHON:
 * Python would use a fixture that yields data
 * C++ uses regular functions that return data
 */
inline std::vector<Expense> createMultipleExpenses(Database& db, int count) {
    // Create test user
    int user_id = Models::createUser(db, "TestUser");
    
    // Get categories
    auto categories = Models::getAllCategories(db);
    if (categories.empty()) {
        return {};
    }
    
    // Create expenses
    std::vector<Expense> expenses;
    for (int i = 0; i < count; ++i) {
        int category_id = categories[i % categories.size()].id;
        std::string date = "2025-10-" + std::to_string(20 + i);
        std::string title = "Expense " + std::to_string(i + 1);
        double amount = 10.0 * (i + 1);
        
        Models::insertExpense(db, date, category_id, title, amount, user_id);
    }
    
    // Fetch and return all expenses
    return Models::fetchExpensesByFilters(db);
}

/**
 * C++ FEATURE: Helper function for expense comparison
 * Compares two expenses for equality
 */
inline bool compareExpenses(const Expense& e1, const Expense& e2) {
    return e1.id == e2.id &&
           e1.date == e2.date &&
           e1.title == e2.title &&
           e1.amount == e2.amount &&
           e1.category_id == e2.category_id &&
           e1.user_id == e2.user_id;
}

/**
 * C++ FEATURE: Helper for creating multiple users
 */
inline std::vector<User> createMultipleUsers(Database& db, int count) {
    std::vector<User> users;
    for (int i = 0; i < count; ++i) {
        std::string name = "User" + std::to_string(i + 1);
        int id = Models::createUser(db, name);
        users.push_back(User(id, name));
    }
    return users;
}

/**
 * C++ FEATURE: Helper for validating expense summary
 */
inline bool validateSummary(const ExpenseSummary& summary, 
                           double expected_total, 
                           int expected_count) {
    const double EPSILON = 0.01;
    return std::abs(summary.total - expected_total) < EPSILON &&
           summary.count == expected_count;
}

#endif // TEST_HELPERS_H

