/**
 * test_functional.cpp
 * Functional tests based on user stories for C++ Expense Tracker
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - User story-based testing
 * - Complete application flow testing
 * - Real-world scenario testing
 * - End-user perspective testing
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../expense_operations.h"
#include "../utils.h"

// ============================================================================
// USER STORY TESTS
// ============================================================================

/**
 * USER STORY: As a user, I can record an expense
 * Given: I have expense details (date, category, title, amount)
 * When: I record the expense
 * Then: The expense is saved and I can view it
 * 
 * C++ FEATURE: Complete workflow with RAII and STL
 */
TEST_F(ModelsTest, UserCanRecordExpense) {
    // User records an expense
    int expense_id = ExpenseOperations::recordExpense(
        *db, "2025-10-25", "Groceries", "Weekly Shopping", 150.0, "JohnDoe"
    );
    
    // Expense is saved
    ASSERT_GT(expense_id, 0);
    
    // User can view the expense
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    ASSERT_EQ(expenses.size(), 1);
    
    // Verify expense details
    EXPECT_EQ(expenses[0].date, "2025-10-25");
    EXPECT_EQ(expenses[0].title, "Weekly Shopping");
    EXPECT_DOUBLE_EQ(expenses[0].amount, 150.0);
    EXPECT_EQ(expenses[0].user_name, "JohnDoe");
    EXPECT_EQ(expenses[0].category_name, "Groceries");
}

/**
 * USER STORY: As a user, I can view expenses by date range
 * Given: I have multiple expenses on different dates
 * When: I filter by date range
 * Then: I see only expenses within that range
 */
TEST_F(ModelsTest, UserCanViewExpensesByDate) {
    // User records expenses on different dates
    ExpenseOperations::recordExpense(*db, "2025-10-20", "Food", "Breakfast", 10.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 20.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-30", "Food", "Dinner", 30.0, "User");
    
    // User filters by date range (Oct 22-28)
    std::vector<Expense> filtered = ExpenseOperations::viewExpensesByDate(
        *db, "2025-10-22", "2025-10-28"
    );
    
    // User sees only expenses in range
    ASSERT_EQ(filtered.size(), 1);
    EXPECT_EQ(filtered[0].title, "Lunch");
    EXPECT_EQ(filtered[0].date, "2025-10-25");
}

/**
 * USER STORY: As a user, I can view expense summary
 * Given: I have recorded multiple expenses
 * When: I view the summary
 * Then: I see total amount, count, and breakdown by category and user
 */
TEST_F(ModelsTest, UserCanViewSummary) {
    // User records multiple expenses
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 25.0, "Alice");
    ExpenseOperations::recordExpense(*db, "2025-10-26", "Transport", "Taxi", 15.0, "Bob");
    ExpenseOperations::recordExpense(*db, "2025-10-27", "Food", "Dinner", 40.0, "Alice");
    
    // User views summary
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    // User sees total and count
    EXPECT_DOUBLE_EQ(summary.total, 80.0);
    EXPECT_EQ(summary.count, 3);
    
    // User sees breakdown by user
    EXPECT_EQ(summary.by_user.size(), 2);
    EXPECT_DOUBLE_EQ(summary.by_user["Alice"], 65.0);
    EXPECT_DOUBLE_EQ(summary.by_user["Bob"], 15.0);
    
    // User sees breakdown by category
    EXPECT_DOUBLE_EQ(summary.by_category["Food"], 65.0);
    EXPECT_DOUBLE_EQ(summary.by_category["Transport"], 15.0);
    
    // User sees individual expenses per user
    EXPECT_EQ(summary.user_expenses["Alice"].size(), 2);
    EXPECT_EQ(summary.user_expenses["Bob"].size(), 1);
}

/**
 * USER STORY: As a user, I can manage categories
 * Given: The system has default categories
 * When: I create a new category
 * Then: I can use it for expenses
 */
TEST_F(ModelsTest, UserCanManageCategories) {
    // System has default categories
    std::vector<Category> categories = Models::getAllCategories(*db);
    size_t initial_count = categories.size();
    EXPECT_GT(initial_count, 0);
    
    // User creates new category
    int new_category_id = Models::createCategory(*db, "CustomCategory");
    EXPECT_GT(new_category_id, 0);
    
    // User can see the new category
    categories = Models::getAllCategories(*db);
    EXPECT_EQ(categories.size(), initial_count + 1);
    
    // User can use new category for expense
    int user_id = createSampleUser();
    int expense_id = Models::insertExpense(*db, "2025-10-25", new_category_id,
                                          "Test Expense", 50.0, user_id);
    EXPECT_GT(expense_id, 0);
    
    // Verify expense has correct category
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ASSERT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].category_name, "CustomCategory");
}

/**
 * USER STORY: As a user, I can manage users
 * Given: I want to track expenses for multiple people
 * When: I record expenses with different user names
 * Then: Each user's expenses are tracked separately
 */
TEST_F(ModelsTest, UserCanManageUsers) {
    // User records expenses for different people
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Alice Lunch", 25.0, "Alice");
    ExpenseOperations::recordExpense(*db, "2025-10-26", "Food", "Bob Lunch", 30.0, "Bob");
    ExpenseOperations::recordExpense(*db, "2025-10-27", "Food", "Alice Dinner", 45.0, "Alice");
    
    // System tracks users separately
    std::vector<User> users = Models::getAllUsers(*db);
    EXPECT_EQ(users.size(), 2);
    
    // User can view expenses by specific user
    auto alice = Models::getUserByName(*db, "Alice");
    ASSERT_TRUE(alice.has_value());
    
    std::vector<Expense> alice_expenses = ExpenseOperations::viewExpensesByUser(*db, alice->id);
    EXPECT_EQ(alice_expenses.size(), 2);
    
    // Verify Alice's total
    ExpenseSummary alice_summary = ExpenseOperations::calculateSummary(*db, &alice_expenses);
    EXPECT_DOUBLE_EQ(alice_summary.total, 70.0);
}

/**
 * USER STORY: As a user, the system prevents invalid data
 * Given: I try to enter invalid expense data
 * When: I submit the data
 * Then: The system rejects it with an error
 * 
 * C++ FEATURE: Exception handling for validation
 */
TEST_F(ModelsTest, ExpenseValidationPreventsInvalidData) {
    // Invalid date format
    EXPECT_THROW(
        ExpenseOperations::recordExpense(*db, "invalid", "Food", "Test", 50.0, "User"),
        std::invalid_argument
    );
    
    // Negative amount
    EXPECT_THROW(
        ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Test", -50.0, "User"),
        std::invalid_argument
    );
    
    // Zero amount
    EXPECT_THROW(
        ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Test", 0.0, "User"),
        std::invalid_argument
    );
    
    // Empty title
    EXPECT_THROW(
        ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "", 50.0, "User"),
        std::invalid_argument
    );
    
    // Empty user name
    EXPECT_THROW(
        ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Test", 50.0, ""),
        std::invalid_argument
    );
    
    // Verify no invalid expenses were created
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    EXPECT_EQ(expenses.size(), 0);
}

/**
 * USER STORY: As a user, I can filter by amount range
 * Given: I have expenses of different amounts
 * When: I filter by amount range
 * Then: I see only expenses within that range
 */
TEST_F(ModelsTest, UserCanFilterByAmountRange) {
    // User records expenses with different amounts
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Cheap", 10.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Medium", 50.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Expensive", 100.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Very Expensive", 200.0, "User");
    
    // User filters by amount range ($40-$120)
    std::vector<Expense> filtered = ExpenseOperations::viewExpensesByAmount(*db, 40.0, 120.0);
    
    // User sees only expenses in range
    ASSERT_EQ(filtered.size(), 2);
    EXPECT_EQ(filtered[0].title, "Medium");
    EXPECT_EQ(filtered[1].title, "Expensive");
}

/**
 * USER STORY: As a user, I can filter by category
 * Given: I have expenses in different categories
 * When: I filter by specific categories
 * Then: I see only expenses in those categories
 */
TEST_F(ModelsTest, UserCanFilterByCategory) {
    // User records expenses in different categories
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 25.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Transport", "Taxi", 15.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Entertainment", "Movie", 20.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Dinner", 40.0, "User");
    
    // User filters by Food category
    std::vector<std::string> categories = {"Food"};
    std::vector<Expense> filtered = ExpenseOperations::viewExpensesByCategory(*db, categories);
    
    // User sees only Food expenses
    ASSERT_EQ(filtered.size(), 2);
    EXPECT_EQ(filtered[0].category_name, "Food");
    EXPECT_EQ(filtered[1].category_name, "Food");
    
    // User filters by multiple categories
    categories = {"Transport", "Entertainment"};
    filtered = ExpenseOperations::viewExpensesByCategory(*db, categories);
    
    ASSERT_EQ(filtered.size(), 2);
}

/**
 * USER STORY: As a user, I can track expenses over time
 * Given: I record expenses over multiple days
 * When: I view my expense history
 * Then: I can see trends and patterns
 */
TEST_F(ModelsTest, UserCanTrackExpensesOverTime) {
    // User records expenses over a week
    ExpenseOperations::recordExpense(*db, "2025-10-20", "Food", "Monday Lunch", 15.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-21", "Food", "Tuesday Lunch", 18.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-22", "Food", "Wednesday Lunch", 20.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-23", "Food", "Thursday Lunch", 17.0, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-24", "Food", "Friday Lunch", 22.0, "User");
    
    // User views all expenses
    std::vector<Expense> all_expenses = ExpenseOperations::viewAllExpenses(*db);
    EXPECT_EQ(all_expenses.size(), 5);
    
    // User calculates weekly summary
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &all_expenses);
    EXPECT_DOUBLE_EQ(summary.total, 92.0);
    EXPECT_EQ(summary.count, 5);
    
    // User can view specific date range
    std::vector<Expense> midweek = ExpenseOperations::viewExpensesByDate(
        *db, "2025-10-22", "2025-10-23"
    );
    EXPECT_EQ(midweek.size(), 2);
    
    ExpenseSummary midweek_summary = ExpenseOperations::calculateSummary(*db, &midweek);
    EXPECT_DOUBLE_EQ(midweek_summary.total, 37.0);
}

/**
 * USER STORY: As a user, multiple users can track expenses independently
 * Given: Multiple people use the system
 * When: Each person records their expenses
 * Then: Each person's data is kept separate and can be viewed individually
 */
TEST_F(ModelsTest, MultipleUsersIndependentTracking) {
    // Alice records her expenses
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Alice Breakfast", 10.0, "Alice");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Transport", "Alice Bus", 5.0, "Alice");
    
    // Bob records his expenses
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Bob Lunch", 20.0, "Bob");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Entertainment", "Bob Movie", 15.0, "Bob");
    
    // Charlie records his expenses
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Charlie Dinner", 30.0, "Charlie");
    
    // Each user can view their own expenses
    auto alice = Models::getUserByName(*db, "Alice");
    auto bob = Models::getUserByName(*db, "Bob");
    auto charlie = Models::getUserByName(*db, "Charlie");
    
    ASSERT_TRUE(alice.has_value());
    ASSERT_TRUE(bob.has_value());
    ASSERT_TRUE(charlie.has_value());
    
    std::vector<Expense> alice_expenses = ExpenseOperations::viewExpensesByUser(*db, alice->id);
    std::vector<Expense> bob_expenses = ExpenseOperations::viewExpensesByUser(*db, bob->id);
    std::vector<Expense> charlie_expenses = ExpenseOperations::viewExpensesByUser(*db, charlie->id);
    
    EXPECT_EQ(alice_expenses.size(), 2);
    EXPECT_EQ(bob_expenses.size(), 2);
    EXPECT_EQ(charlie_expenses.size(), 1);
    
    // Each user's totals are independent
    ExpenseSummary alice_summary = ExpenseOperations::calculateSummary(*db, &alice_expenses);
    ExpenseSummary bob_summary = ExpenseOperations::calculateSummary(*db, &bob_expenses);
    ExpenseSummary charlie_summary = ExpenseOperations::calculateSummary(*db, &charlie_expenses);
    
    EXPECT_DOUBLE_EQ(alice_summary.total, 15.0);
    EXPECT_DOUBLE_EQ(bob_summary.total, 35.0);
    EXPECT_DOUBLE_EQ(charlie_summary.total, 30.0);
    
    // Overall summary shows all users
    std::vector<Expense> all_expenses = ExpenseOperations::viewAllExpenses(*db);
    ExpenseSummary overall = ExpenseOperations::calculateSummary(*db, &all_expenses);
    
    EXPECT_DOUBLE_EQ(overall.total, 80.0);
    EXPECT_EQ(overall.count, 5);
    EXPECT_EQ(overall.by_user.size(), 3);
}

/**
 * USER STORY: As a user, I can see formatted output
 * Given: I have recorded expenses
 * When: I view expenses or summary
 * Then: The output is well-formatted and readable
 * 
 * C++ FEATURE: String formatting with Utils class
 */
TEST_F(ModelsTest, UserSeesFormattedOutput) {
    // User records expenses
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 25.50, "User");
    ExpenseOperations::recordExpense(*db, "2025-10-26", "Transport", "Taxi", 15.75, "User");
    
    // User views formatted expense list
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    std::string expense_output = Utils::formatExpenseOutput(expenses);
    
    // Output contains formatted data
    EXPECT_NE(expense_output.find("$25.50"), std::string::npos);
    EXPECT_NE(expense_output.find("$15.75"), std::string::npos);
    EXPECT_NE(expense_output.find("Lunch"), std::string::npos);
    EXPECT_NE(expense_output.find("Taxi"), std::string::npos);
    
    // User views formatted summary
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    std::string summary_output = Utils::formatSummaryOutput(summary);
    
    // Summary contains formatted totals
    EXPECT_NE(summary_output.find("$41.25"), std::string::npos);
    EXPECT_NE(summary_output.find("EXPENSE SUMMARY"), std::string::npos);
    EXPECT_NE(summary_output.find("%"), std::string::npos);  // Percentages
}

/**
 * USER STORY: As a user, I can work with the system safely
 * Given: The system uses proper resource management
 * When: I perform operations
 * Then: Resources are properly cleaned up automatically
 * 
 * C++ FEATURE: RAII ensures automatic cleanup
 */
TEST(FunctionalTest, UserBenefitsFromRAII) {
    std::string test_path = "test_functional_raii.db";
    
    // User performs operations in a scope
    {
        Database db(test_path);
        db.initialize();
        
        // User records expenses
        ExpenseOperations::recordExpense(db, "2025-10-25", "Food", "Test", 50.0, "User");
        
        // Resources automatically cleaned up when scope ends
    }
    
    // User can reopen and continue working
    {
        Database db(test_path);
        std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(db);
        EXPECT_EQ(expenses.size(), 1);
    }
    
    // Cleanup
    std::remove(test_path.c_str());
}

