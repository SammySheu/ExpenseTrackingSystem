/**
 * test_expense_operations.cpp
 * Unit tests for ExpenseOperations class (Business Logic Layer)
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - Business logic testing
 * - STL container operations
 * - Struct and map testing
 * - Auto-creation logic testing
 * - Summary calculation testing
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../expense_operations.h"

// ============================================================================
// RECORD EXPENSE TESTS
// ============================================================================

/**
 * TEST: RecordExpense succeeds with valid data
 * C++ FEATURE: Multiple parameter types in static method
 */
TEST_F(ModelsTest, RecordExpenseSucceeds) {
    // Get first category
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    int expense_id = ExpenseOperations::recordExpense(
        *db, "2025-10-25", categories[0].name, "Test Expense", 50.0, "TestUser"
    );
    
    EXPECT_GT(expense_id, 0);
}

/**
 * TEST: RecordExpense creates new user if not exists
 * Tests auto-creation logic
 */
TEST_F(ModelsTest, RecordExpenseCreatesNewUser) {
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    // Record expense with new user
    ExpenseOperations::recordExpense(
        *db, "2025-10-25", categories[0].name, "Test", 50.0, "NewUser"
    );
    
    // Verify user was created
    auto user = Models::getUserByName(*db, "NewUser");
    EXPECT_TRUE(user.has_value());
}

/**
 * TEST: RecordExpense creates new category if not exists
 * Tests auto-creation logic
 */
TEST_F(ModelsTest, RecordExpenseCreatesNewCategory) {
    int user_id = createSampleUser();
    
    // Record expense with new category
    ExpenseOperations::recordExpense(
        *db, "2025-10-25", "NewCategory", "Test", 50.0, "TestUser"
    );
    
    // Verify category was created
    auto category = Models::getCategoryByName(*db, "NewCategory");
    EXPECT_TRUE(category.has_value());
}

/**
 * TEST: RecordExpense with invalid date throws
 */
TEST_F(ModelsTest, RecordExpenseInvalidDateThrows) {
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    EXPECT_THROW(
        ExpenseOperations::recordExpense(
            *db, "invalid-date", categories[0].name, "Test", 50.0, "User"
        ),
        std::invalid_argument
    );
}

/**
 * TEST: RecordExpense with invalid amount throws
 */
TEST_F(ModelsTest, RecordExpenseInvalidAmountThrows) {
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    EXPECT_THROW(
        ExpenseOperations::recordExpense(
            *db, "2025-10-25", categories[0].name, "Test", -50.0, "User"
        ),
        std::invalid_argument
    );
    
    EXPECT_THROW(
        ExpenseOperations::recordExpense(
            *db, "2025-10-25", categories[0].name, "Test", 0.0, "User"
        ),
        std::invalid_argument
    );
}

/**
 * TEST: RecordExpense with empty title throws
 */
TEST_F(ModelsTest, RecordExpenseEmptyTitleThrows) {
    auto categories = Models::getAllCategories(*db);
    ASSERT_FALSE(categories.empty());
    
    EXPECT_THROW(
        ExpenseOperations::recordExpense(
            *db, "2025-10-25", categories[0].name, "", 50.0, "User"
        ),
        std::invalid_argument
    );
}

// ============================================================================
// VIEW EXPENSES TESTS
// ============================================================================

/**
 * TEST: ViewExpensesByDate with no filters
 * C++ FEATURE: Default parameter values (empty strings)
 */
TEST_F(ModelsTest, ViewExpensesByDateNoFilters) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Exp1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Exp2", 20.0, user_id);
    
    // C++ FEATURE: Default parameters (empty strings)
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByDate(*db);
    
    EXPECT_EQ(expenses.size(), 2);
}

/**
 * TEST: ViewExpensesByDate with min date only
 */
TEST_F(ModelsTest, ViewExpensesByDateMinOnly) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-20", category_id, "Early", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Late", 20.0, user_id);
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByDate(*db, "2025-10-23");
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Late");
}

/**
 * TEST: ViewExpensesByDate with max date only
 */
TEST_F(ModelsTest, ViewExpensesByDateMaxOnly) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-20", category_id, "Early", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Late", 20.0, user_id);
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByDate(*db, "", "2025-10-23");
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Early");
}

/**
 * TEST: ViewExpensesByDate with date range
 */
TEST_F(ModelsTest, ViewExpensesByDateRange) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-20", category_id, "Before", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "During", 20.0, user_id);
    Models::insertExpense(*db, "2025-10-30", category_id, "After", 30.0, user_id);
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByDate(
        *db, "2025-10-22", "2025-10-28"
    );
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "During");
}

/**
 * TEST: ViewExpensesByAmount with amount range
 * C++ FEATURE: Double parameters with default values
 */
TEST_F(ModelsTest, ViewExpensesByAmountRange) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Cheap", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Medium", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-25", category_id, "Expensive", 100.0, user_id);
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByAmount(*db, 30.0, 70.0);
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Medium");
}

/**
 * TEST: ViewExpensesByCategory with single category
 * C++ FEATURE: Vector parameter
 */
TEST_F(ModelsTest, ViewExpensesByCategorySingle) {
    int user_id = createSampleUser();
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 2);
    
    Models::insertExpense(*db, "2025-10-25", categories[0].id, "Cat1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", categories[1].id, "Cat2", 20.0, user_id);
    
    // C++ FEATURE: std::vector initialization
    std::vector<std::string> category_names = {categories[0].name};
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByCategory(*db, category_names);
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Cat1");
}

/**
 * TEST: ViewExpensesByCategory with multiple categories
 */
TEST_F(ModelsTest, ViewExpensesByCategoryMultiple) {
    int user_id = createSampleUser();
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 3);
    
    Models::insertExpense(*db, "2025-10-25", categories[0].id, "Cat1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-25", categories[1].id, "Cat2", 20.0, user_id);
    Models::insertExpense(*db, "2025-10-25", categories[2].id, "Cat3", 30.0, user_id);
    
    std::vector<std::string> category_names = {categories[0].name, categories[2].name};
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByCategory(*db, category_names);
    
    EXPECT_EQ(expenses.size(), 2);
}

/**
 * TEST: ViewExpensesByUser with valid user ID
 */
TEST_F(ModelsTest, ViewExpensesByUserValid) {
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "User1 Exp", 10.0, user1);
    Models::insertExpense(*db, "2025-10-25", category_id, "User2 Exp", 20.0, user2);
    
    std::vector<Expense> expenses = ExpenseOperations::viewExpensesByUser(*db, user1);
    
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].user_name, "User1");
}

/**
 * TEST: ViewAllExpenses returns all expenses
 */
TEST_F(ModelsTest, ViewAllExpenses) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Exp1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Exp2", 20.0, user_id);
    Models::insertExpense(*db, "2025-10-27", category_id, "Exp3", 30.0, user_id);
    
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    
    EXPECT_EQ(expenses.size(), 3);
}

// ============================================================================
// SUMMARY TESTS
// ============================================================================

/**
 * TEST: CalculateSummary with empty data
 * C++ FEATURE: Struct initialization and testing
 * 
 * CONTRAST WITH PYTHON:
 * Python: Returns dict with default values
 * C++: Returns struct with zero-initialized values
 */
TEST_F(ModelsTest, CalculateSummaryEmpty) {
    std::vector<Expense> expenses;
    
    // C++ FEATURE: Pointer to vector for optional parameter
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    EXPECT_DOUBLE_EQ(summary.total, 0.0);
    EXPECT_EQ(summary.count, 0);
    EXPECT_TRUE(summary.by_category.empty());
    EXPECT_TRUE(summary.by_user.empty());
}

/**
 * TEST: CalculateSummary with single user
 * Tests summary calculation for one user
 */
TEST_F(ModelsTest, CalculateSummarySingleUser) {
    int user_id = createSampleUser("TestUser");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Exp1", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Exp2", 30.0, user_id);
    
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    EXPECT_DOUBLE_EQ(summary.total, 80.0);
    EXPECT_EQ(summary.count, 2);
    
    // C++ FEATURE: std::map operations
    EXPECT_EQ(summary.by_user.size(), 1);
    EXPECT_DOUBLE_EQ(summary.by_user["TestUser"], 80.0);
}

/**
 * TEST: CalculateSummary with multiple users
 * C++ FEATURE: Map with multiple entries
 */
TEST_F(ModelsTest, CalculateSummaryMultipleUsers) {
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "U1 Exp1", 50.0, user1);
    Models::insertExpense(*db, "2025-10-26", category_id, "U1 Exp2", 30.0, user1);
    Models::insertExpense(*db, "2025-10-27", category_id, "U2 Exp1", 20.0, user2);
    
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    EXPECT_DOUBLE_EQ(summary.total, 100.0);
    EXPECT_EQ(summary.count, 3);
    EXPECT_EQ(summary.by_user.size(), 2);
    EXPECT_DOUBLE_EQ(summary.by_user["User1"], 80.0);
    EXPECT_DOUBLE_EQ(summary.by_user["User2"], 20.0);
}

/**
 * TEST: CalculateSummary by category
 * C++ FEATURE: Map operations and aggregation
 */
TEST_F(ModelsTest, CalculateSummaryByCategory) {
    int user_id = createSampleUser();
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 2);
    
    Models::insertExpense(*db, "2025-10-25", categories[0].id, "Cat1 Exp1", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-26", categories[0].id, "Cat1 Exp2", 30.0, user_id);
    Models::insertExpense(*db, "2025-10-27", categories[1].id, "Cat2 Exp1", 20.0, user_id);
    
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    // C++ FEATURE: Map with string keys
    EXPECT_GE(summary.by_category.size(), 2);
    EXPECT_DOUBLE_EQ(summary.by_category[categories[0].name], 80.0);
    EXPECT_DOUBLE_EQ(summary.by_category[categories[1].name], 20.0);
}

/**
 * TEST: CalculateSummary user expenses grouped
 * C++ FEATURE: Nested containers (map<string, vector<Expense>>)
 * 
 * CONTRAST WITH PYTHON:
 * Python: dict[str, list[dict]]
 * C++: std::map<std::string, std::vector<Expense>>
 */
TEST_F(ModelsTest, CalculateSummaryUserExpensesGrouped) {
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "U1 Exp1", 50.0, user1);
    Models::insertExpense(*db, "2025-10-26", category_id, "U1 Exp2", 30.0, user1);
    Models::insertExpense(*db, "2025-10-27", category_id, "U2 Exp1", 20.0, user2);
    
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    // C++ FEATURE: Nested container access
    EXPECT_EQ(summary.user_expenses.size(), 2);
    EXPECT_EQ(summary.user_expenses["User1"].size(), 2);
    EXPECT_EQ(summary.user_expenses["User2"].size(), 1);
    
    // Verify expense details - check that both expenses are present (order may vary)
    const auto& user1_expenses = summary.user_expenses["User1"];
    bool has_exp1 = false, has_exp2 = false;
    for (const auto& exp : user1_expenses) {
        if (exp.title == "U1 Exp1") has_exp1 = true;
        if (exp.title == "U1 Exp2") has_exp2 = true;
    }
    EXPECT_TRUE(has_exp1);
    EXPECT_TRUE(has_exp2);
    EXPECT_EQ(summary.user_expenses["User2"][0].title, "U2 Exp1");
}

/**
 * TEST: CalculateSummary with nullptr (uses all expenses)
 * C++ FEATURE: nullptr for optional pointer parameter
 */
TEST_F(ModelsTest, CalculateSummaryWithNullptr) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Exp1", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Exp2", 30.0, user_id);
    
    // C++ FEATURE: nullptr for optional parameter - fetches all expenses internally
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, nullptr);
    
    EXPECT_DOUBLE_EQ(summary.total, 80.0);
    EXPECT_EQ(summary.count, 2);
}

