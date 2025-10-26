/**
 * test_models.cpp
 * Unit tests for Models class (Data Access Layer)
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - STL vector testing
 * - optional testing
 * - Exception testing with EXPECT_THROW
 * - Test fixture inheritance
 * - Pass by reference testing
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../models.h"

using namespace std;

// ============================================================================
// USER TESTS
// ============================================================================

/**
 * TEST: Create user returns valid ID
 * C++ FEATURE: Static method call with reference parameter
 */
TEST_F(ModelsTest, CreateUserReturnsValidId) {
    // C++ FEATURE: Pass by reference (db is Database&)
    int user_id = Models::createUser(*db, "TestUser");
    
    EXPECT_GT(user_id, 0);
}

/**
 * TEST: Create user with duplicate name throws exception
 * C++ FEATURE: Exception testing with EXPECT_THROW
 * 
 * CONTRAST WITH PYTHON:
 * Python: pytest.raises(Exception)
 * C++: EXPECT_THROW(code, exception_type)
 */
TEST_F(ModelsTest, CreateUserWithDuplicateNameThrows) {
    Models::createUser(*db, "DuplicateUser");
    
    // C++ FEATURE: EXPECT_THROW macro for exception testing
    EXPECT_THROW(
        Models::createUser(*db, "DuplicateUser"),
        runtime_error
    );
}

/**
 * TEST: Create user with empty name throws exception
 * Tests validation logic
 */
TEST_F(ModelsTest, CreateUserWithEmptyNameThrows) {
    EXPECT_THROW(
        Models::createUser(*db, ""),
        invalid_argument
    );
    
    EXPECT_THROW(
        Models::createUser(*db, "   "),
        invalid_argument
    );
}

/**
 * TEST: GetAllUsers returns empty vector initially
 * C++ FEATURE: STL vector testing
 * 
 * CONTRAST WITH PYTHON:
 * Python: Returns empty list []
 * C++: Returns empty vector<User>
 */
TEST_F(ModelsTest, GetAllUsersReturnsEmptyVector) {
    // C++ FEATURE: STL vector return type
    vector<User> users = Models::getAllUsers(*db);
    
    EXPECT_TRUE(users.empty());
    EXPECT_EQ(users.size(), 0);
}

/**
 * TEST: GetAllUsers returns all users
 * Tests vector population
 */
TEST_F(ModelsTest, GetAllUsersReturnsAllUsers) {
    // Create multiple users
    Models::createUser(*db, "User1");
    Models::createUser(*db, "User2");
    Models::createUser(*db, "User3");
    
    // C++ FEATURE: STL vector with struct elements
    vector<User> users = Models::getAllUsers(*db);
    
    EXPECT_EQ(users.size(), 3);
    EXPECT_EQ(users[0].name, "User1");
    EXPECT_EQ(users[1].name, "User2");
    EXPECT_EQ(users[2].name, "User3");
}

/**
 * TEST: GetUserByName returns optional with value
 * C++ FEATURE: optional testing (C++17)
 * 
 * CONTRAST WITH PYTHON:
 * Python: Returns None or dict
 * C++: Returns optional<User>
 */
TEST_F(ModelsTest, GetUserByNameReturnsOptional) {
    Models::createUser(*db, "FindMe");
    
    // C++ FEATURE: optional return type
    optional<User> user = Models::getUserByName(*db, "FindMe");
    
    // C++ FEATURE: has_value() checks if optional contains value
    EXPECT_TRUE(user.has_value());
    
    // C++ FEATURE: value() or operator* to access contained value
    EXPECT_EQ(user.value().name, "FindMe");
}

/**
 * TEST: GetUserByName returns nullopt when not found
 * C++ FEATURE: nullopt testing
 */
TEST_F(ModelsTest, GetUserByNameReturnsNulloptWhenNotFound) {
    optional<User> user = Models::getUserByName(*db, "NonExistent");
    
    // C++ FEATURE: optional can be empty (nullopt)
    EXPECT_FALSE(user.has_value());
}

// ============================================================================
// CATEGORY TESTS
// ============================================================================

/**
 * TEST: Create category returns valid ID
 */
TEST_F(ModelsTest, CreateCategoryReturnsValidId) {
    int category_id = Models::createCategory(*db, "TestCategory");
    
    EXPECT_GT(category_id, 0);
}

/**
 * TEST: Create category with duplicate throws
 */
TEST_F(ModelsTest, CreateCategoryWithDuplicateThrows) {
    Models::createCategory(*db, "DuplicateCategory");
    
    EXPECT_THROW(
        Models::createCategory(*db, "DuplicateCategory"),
        runtime_error
    );
}

/**
 * TEST: GetAllCategories includes defaults
 * Tests that default categories are created during initialization
 */
TEST_F(ModelsTest, GetAllCategoriesIncludesDefaults) {
    // C++ FEATURE: STL vector of Category structs
    vector<Category> categories = Models::getAllCategories(*db);
    
    // Should have default categories
    EXPECT_GT(categories.size(), 0);
}

/**
 * TEST: GetCategoryByName returns optional
 */
TEST_F(ModelsTest, GetCategoryByNameReturnsOptional) {
    // Get first default category
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    string category_name = categories[0].name;
    
    optional<Category> category = Models::getCategoryByName(*db, category_name);
    
    EXPECT_TRUE(category.has_value());
    EXPECT_EQ(category.value().name, category_name);
}

// ============================================================================
// EXPENSE TESTS
// ============================================================================

/**
 * TEST: Insert expense returns valid ID
 * C++ FEATURE: Multiple parameter types (string, int, double)
 */
TEST_F(ModelsTest, InsertExpenseReturnsValidId) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    // C++ FEATURE: Static method with multiple typed parameters
    int expense_id = Models::insertExpense(*db, "2025-10-25", category_id,
                                          "Test Expense", 50.0, user_id);
    
    EXPECT_GT(expense_id, 0);
}

/**
 * TEST: Insert expense with invalid category ID throws
 * Tests foreign key constraint
 */
TEST_F(ModelsTest, InsertExpenseWithInvalidCategoryIdThrows) {
    int user_id = createSampleUser();
    
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", 99999,
                            "Invalid Category", 50.0, user_id),
        runtime_error
    );
}

/**
 * TEST: Insert expense with invalid user ID throws
 */
TEST_F(ModelsTest, InsertExpenseWithInvalidUserIdThrows) {
    int category_id = createSampleCategory();
    
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", category_id,
                            "Invalid User", 50.0, 99999),
        runtime_error
    );
}

/**
 * TEST: Insert expense with empty title throws
 */
TEST_F(ModelsTest, InsertExpenseWithEmptyTitleThrows) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", category_id,
                            "", 50.0, user_id),
        invalid_argument
    );
}

/**
 * TEST: Insert expense with negative amount throws
 */
TEST_F(ModelsTest, InsertExpenseWithNegativeAmountThrows) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", category_id,
                            "Negative Amount", -50.0, user_id),
        invalid_argument
    );
}

/**
 * TEST: FetchExpensesByFilters with no filters
 * C++ FEATURE: Default nullptr parameters
 * 
 * CONTRAST WITH PYTHON:
 * Python: Uses None as default
 * C++: Uses nullptr for pointer parameters
 */
TEST_F(ModelsTest, FetchExpensesByFiltersNoFilters) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Expense 1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Expense 2", 20.0, user_id);
    
    // C++ FEATURE: nullptr for optional parameters
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    
    EXPECT_EQ(expenses.size(), 2);
}

/**
 * TEST: FetchExpensesByFilters with date range
 * C++ FEATURE: Pointer parameters for optional values
 */
TEST_F(ModelsTest, FetchExpensesByFiltersDateRange) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-20", category_id, "Early", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Middle", 20.0, user_id);
    Models::insertExpense(*db, "2025-10-30", category_id, "Late", 30.0, user_id);
    
    // C++ FEATURE: Pass pointers for optional parameters
    string min_date = "2025-10-22";
    string max_date = "2025-10-28";
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(
        *db, &min_date, &max_date
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Middle");
}

/**
 * TEST: FetchExpensesByFilters with amount range
 */
TEST_F(ModelsTest, FetchExpensesByFiltersAmountRange) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Cheap", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Medium", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Expensive", 100.0, user_id);
    
    double min_amount = 30.0;
    double max_amount = 70.0;
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(
        *db, nullptr, nullptr, &min_amount, &max_amount
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Medium");
}

/**
 * TEST: FetchExpensesByFilters with category IDs
 * C++ FEATURE: Vector parameter for multiple values
 */
TEST_F(ModelsTest, FetchExpensesByFiltersCategoryIds) {
    int user_id = createSampleUser();
    
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 2);
    
    int cat1 = categories[0].id;
    int cat2 = categories[1].id;
    
    Models::insertExpense(*db, "2025-10-25", cat1, "Cat1 Expense", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", cat2, "Cat2 Expense", 20.0, user_id);
    
    // C++ FEATURE: vector for multiple values
    vector<int> category_ids = {cat1};
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(
        *db, nullptr, nullptr, nullptr, nullptr, &category_ids
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Cat1 Expense");
}

/**
 * TEST: FetchExpensesByFilters with user ID
 */
TEST_F(ModelsTest, FetchExpensesByFiltersUserId) {
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "User1 Expense", 10.0, user1);
    Models::insertExpense(*db, "2025-10-25", category_id, "User2 Expense", 20.0, user2);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(
        *db, nullptr, nullptr, nullptr, nullptr, nullptr, &user1
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].user_name, "User1");
}

/**
 * TEST: FetchExpensesByFilters with multiple filters
 * Tests combining multiple filter criteria
 */
TEST_F(ModelsTest, FetchExpensesByFiltersMultiple) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-20", category_id, "Match", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "No Match Date", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-22", category_id, "No Match Amount", 100.0, user_id);
    
    string min_date = "2025-10-19";
    string max_date = "2025-10-21";
    double min_amount = 40.0;
    double max_amount = 60.0;
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(
        *db, &min_date, &max_date, &min_amount, &max_amount
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Match");
}

/**
 * TEST: Expense struct contains joined data
 * C++ FEATURE: Struct with multiple fields
 */
TEST_F(ModelsTest, ExpenseContainsJoinedData) {
    int user_id = createSampleUser("JohnDoe");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Test", 50.0, user_id);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    
    ASSERT_EQ(expenses.size(), 1);
    
    // C++ FEATURE: Access struct fields directly
    EXPECT_EQ(expenses[0].user_name, "JohnDoe");
    EXPECT_FALSE(expenses[0].category_name.empty());
    EXPECT_EQ(expenses[0].amount, 50.0);
}

