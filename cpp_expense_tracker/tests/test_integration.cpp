/**
 * test_integration.cpp
 * Integration tests for C++ Expense Tracker
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - End-to-end workflow testing
 * - Module interaction testing
 * - RAII and resource management testing
 * - Transaction consistency testing
 * - Memory management across operations
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../expense_operations.h"

// ============================================================================
// END-TO-END WORKFLOW TESTS
// ============================================================================

/**
 * TEST: Complete expense workflow
 * Tests: Create user → Create category → Insert expense → View → Calculate summary
 * 
 * C++ FEATURE: Complete workflow with RAII and STL containers
 */
TEST_F(ModelsTest, CompleteExpenseWorkflow) {
    // Step 1: Record expense (auto-creates user and category)
    int expense_id = ExpenseOperations::recordExpense(
        *db, "2025-10-25", "Groceries", "Weekly Shopping", 150.0, "JohnDoe"
    );
    
    EXPECT_GT(expense_id, 0);
    
    // Step 2: Verify user was created
    auto user = Models::getUserByName(*db, "JohnDoe");
    ASSERT_TRUE(user.has_value());
    
    // Step 3: Verify category was created
    auto category = Models::getCategoryByName(*db, "Groceries");
    ASSERT_TRUE(category.has_value());
    
    // Step 4: View all expenses
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    ASSERT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Weekly Shopping");
    EXPECT_DOUBLE_EQ(expenses[0].amount, 150.0);
    
    // Step 5: Calculate summary
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    EXPECT_DOUBLE_EQ(summary.total, 150.0);
    EXPECT_EQ(summary.count, 1);
    EXPECT_EQ(summary.by_user.size(), 1);
    EXPECT_DOUBLE_EQ(summary.by_user["JohnDoe"], 150.0);
}

/**
 * TEST: Multiple users multiple expenses
 * Tests complex scenario with multiple users and categories
 * 
 * C++ FEATURE: STL container operations across multiple modules
 */
TEST_F(ModelsTest, MultipleUsersMultipleExpenses) {
    // Create multiple expenses for different users
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 25.0, "Alice");
    ExpenseOperations::recordExpense(*db, "2025-10-26", "Transport", "Taxi", 15.0, "Bob");
    ExpenseOperations::recordExpense(*db, "2025-10-27", "Food", "Dinner", 40.0, "Alice");
    ExpenseOperations::recordExpense(*db, "2025-10-28", "Entertainment", "Movie", 20.0, "Charlie");
    
    // Verify all expenses were created
    std::vector<Expense> all_expenses = ExpenseOperations::viewAllExpenses(*db);
    EXPECT_EQ(all_expenses.size(), 4);
    
    // Verify users were created
    std::vector<User> users = Models::getAllUsers(*db);
    EXPECT_EQ(users.size(), 3);
    
    // Calculate summary
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &all_expenses);
    EXPECT_DOUBLE_EQ(summary.total, 100.0);
    EXPECT_EQ(summary.count, 4);
    
    // C++ FEATURE: Map operations
    EXPECT_EQ(summary.by_user.size(), 3);
    EXPECT_DOUBLE_EQ(summary.by_user["Alice"], 65.0);
    EXPECT_DOUBLE_EQ(summary.by_user["Bob"], 15.0);
    EXPECT_DOUBLE_EQ(summary.by_user["Charlie"], 20.0);
    
    // Verify category breakdown
    EXPECT_GE(summary.by_category.size(), 3);
    EXPECT_DOUBLE_EQ(summary.by_category["Food"], 65.0);
    EXPECT_DOUBLE_EQ(summary.by_category["Transport"], 15.0);
    EXPECT_DOUBLE_EQ(summary.by_category["Entertainment"], 20.0);
}

/**
 * TEST: Filter and summary workflow
 * Tests chained operations: Insert → Filter → Calculate Summary
 * 
 * C++ FEATURE: Chaining operations with STL containers
 */
TEST_F(ModelsTest, FilterAndSummaryWorkflow) {
    // Create expenses with different dates and amounts
    ExpenseOperations::recordExpense(*db, "2025-10-20", "Food", "Breakfast", 10.0, "User1");
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Lunch", 50.0, "User1");
    ExpenseOperations::recordExpense(*db, "2025-10-30", "Food", "Dinner", 100.0, "User1");
    
    // Filter by date range
    std::vector<Expense> filtered = ExpenseOperations::viewExpensesByDate(
        *db, "2025-10-22", "2025-10-28"
    );
    
    EXPECT_EQ(filtered.size(), 1);
    EXPECT_EQ(filtered[0].title, "Lunch");
    
    // Calculate summary of filtered expenses
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &filtered);
    EXPECT_DOUBLE_EQ(summary.total, 50.0);
    EXPECT_EQ(summary.count, 1);
    
    // Filter by amount range
    filtered = ExpenseOperations::viewExpensesByAmount(*db, 40.0, 150.0);
    EXPECT_EQ(filtered.size(), 2);
    
    // Calculate summary of amount-filtered expenses
    summary = ExpenseOperations::calculateSummary(*db, &filtered);
    EXPECT_DOUBLE_EQ(summary.total, 150.0);
    EXPECT_EQ(summary.count, 2);
}

/**
 * TEST: Database transaction consistency
 * Tests that all operations maintain data integrity
 * 
 * C++ FEATURE: Transaction consistency with SQLite
 */
TEST_F(ModelsTest, DatabaseTransactionConsistency) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    // Insert multiple expenses
    for (int i = 1; i <= 10; ++i) {
        Models::insertExpense(*db, "2025-10-25", category_id, 
                            "Expense " + std::to_string(i), 
                            10.0 * i, user_id);
    }
    
    // Verify count
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    EXPECT_EQ(expenses.size(), 10);
    
    // Verify total amount
    double total = 0.0;
    for (const auto& expense : expenses) {
        total += expense.amount;
    }
    EXPECT_DOUBLE_EQ(total, 550.0);  // 10+20+30+...+100 = 550
    
    // Verify summary calculation matches
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    EXPECT_DOUBLE_EQ(summary.total, 550.0);
    EXPECT_EQ(summary.count, 10);
}

/**
 * TEST: Foreign key constraints enforced
 * Tests that database maintains referential integrity
 * 
 * C++ FEATURE: Exception handling for constraint violations
 */
TEST_F(ModelsTest, ForeignKeyConstraintsEnforced) {
    int user_id = createSampleUser();
    
    // Try to insert expense with invalid category ID
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", 99999, "Invalid", 50.0, user_id),
        std::runtime_error
    );
    
    // Try to insert expense with invalid user ID
    int category_id = createSampleCategory();
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", category_id, "Invalid", 50.0, 99999),
        std::runtime_error
    );
    
    // Verify no expenses were created
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    EXPECT_EQ(expenses.size(), 0);
}

/**
 * TEST: Memory management across operations
 * Tests that STL containers properly manage memory
 * 
 * C++ FEATURE: STL automatic memory management
 * 
 * CONTRAST WITH PYTHON:
 * Python: Garbage collector handles cleanup
 * C++: STL containers use RAII for automatic cleanup
 */
TEST_F(ModelsTest, MemoryManagementAcrossOperations) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    // Create large number of expenses in nested scope
    {
        std::vector<Expense> temp_expenses;
        for (int i = 0; i < 100; ++i) {
            Models::insertExpense(*db, "2025-10-25", category_id,
                                "Expense " + std::to_string(i), 10.0, user_id);
        }
        
        temp_expenses = Models::fetchExpensesByFilters(*db);
        EXPECT_EQ(temp_expenses.size(), 100);
        
        // temp_expenses destroyed here (RAII cleanup)
    }
    
    // Verify expenses still in database
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    EXPECT_EQ(expenses.size(), 100);
    
    // Calculate summary (tests map memory management)
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    EXPECT_EQ(summary.count, 100);
    EXPECT_DOUBLE_EQ(summary.total, 1000.0);
}

/**
 * TEST: RAII cleanup works correctly
 * Tests that Database destructor properly closes connections
 * 
 * C++ FEATURE: RAII pattern in practice
 */
TEST(IntegrationTest, RAIICleanupWorks) {
    std::string test_path = "test_raii_integration.db";
    
    {
        // Create database in nested scope
        Database db(test_path);
        db.initialize();
        
        // Create some data
        int user_id = Models::createUser(db, "TestUser");
        auto categories = Models::getAllCategories(db);
        ASSERT_FALSE(categories.empty());
        
        Models::insertExpense(db, "2025-10-25", categories[0].id, 
                            "Test", 50.0, user_id);
        
        // db destructor called here
    }
    
    // Verify file exists
    EXPECT_TRUE(Database::exists(test_path));
    
    // Reopen and verify data persists
    {
        Database db(test_path);
        std::vector<Expense> expenses = Models::fetchExpensesByFilters(db);
        EXPECT_EQ(expenses.size(), 1);
    }
    
    // Cleanup
    std::remove(test_path.c_str());
}

/**
 * TEST: Multiple database operations in sequence
 * Tests that operations can be chained without issues
 */
TEST_F(ModelsTest, MultipleOperationsInSequence) {
    // Operation 1: Create users
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    
    // Operation 2: Get categories
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 2);
    
    // Operation 3: Insert expenses
    Models::insertExpense(*db, "2025-10-25", categories[0].id, "Exp1", 50.0, user1);
    Models::insertExpense(*db, "2025-10-26", categories[1].id, "Exp2", 30.0, user2);
    
    // Operation 4: Fetch by different filters
    std::vector<Expense> by_user1 = ExpenseOperations::viewExpensesByUser(*db, user1);
    std::vector<Expense> by_user2 = ExpenseOperations::viewExpensesByUser(*db, user2);
    
    EXPECT_EQ(by_user1.size(), 1);
    EXPECT_EQ(by_user2.size(), 1);
    
    // Operation 5: Calculate summaries
    ExpenseSummary summary1 = ExpenseOperations::calculateSummary(*db, &by_user1);
    ExpenseSummary summary2 = ExpenseOperations::calculateSummary(*db, &by_user2);
    
    EXPECT_DOUBLE_EQ(summary1.total, 50.0);
    EXPECT_DOUBLE_EQ(summary2.total, 30.0);
}

/**
 * TEST: Exception safety across modules
 * Tests that exceptions don't leave system in inconsistent state
 * 
 * C++ FEATURE: Exception safety with RAII
 */
TEST_F(ModelsTest, ExceptionSafetyAcrossModules) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    // Insert valid expense
    Models::insertExpense(*db, "2025-10-25", category_id, "Valid", 50.0, user_id);
    
    // Try to insert invalid expense with empty title (should throw)
    // Note: insertExpense now validates at the Models layer
    EXPECT_THROW(
        Models::insertExpense(*db, "2025-10-25", category_id, "", 50.0, user_id),
        std::invalid_argument
    );
    
    // Verify database is still consistent
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    EXPECT_EQ(expenses.size(), 1);
    EXPECT_EQ(expenses[0].title, "Valid");
}

/**
 * TEST: STL container lifetime management
 * Tests that containers properly manage their elements
 * 
 * C++ FEATURE: Container lifetime and element management
 */
TEST_F(ModelsTest, STLContainerLifetimeManagement) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    // Create vector in nested scope
    {
        std::vector<int> expense_ids;
        for (int i = 0; i < 5; ++i) {
            int id = Models::insertExpense(*db, "2025-10-25", category_id,
                                          "Expense " + std::to_string(i), 10.0, user_id);
            expense_ids.push_back(id);
        }
        
        EXPECT_EQ(expense_ids.size(), 5);
        // expense_ids destroyed here
    }
    
    // Verify expenses still exist in database
    std::vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    EXPECT_EQ(expenses.size(), 5);
}

/**
 * TEST: Nested container operations
 * Tests map<string, vector<Expense>> operations
 * 
 * C++ FEATURE: Nested STL containers
 */
TEST_F(ModelsTest, NestedContainerOperations) {
    // Create expenses for multiple users
    ExpenseOperations::recordExpense(*db, "2025-10-25", "Food", "Exp1", 10.0, "User1");
    ExpenseOperations::recordExpense(*db, "2025-10-26", "Food", "Exp2", 20.0, "User1");
    ExpenseOperations::recordExpense(*db, "2025-10-27", "Food", "Exp3", 30.0, "User2");
    
    // Get summary with nested containers
    std::vector<Expense> expenses = ExpenseOperations::viewAllExpenses(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    // C++ FEATURE: Access nested container
    EXPECT_EQ(summary.user_expenses.size(), 2);
    EXPECT_EQ(summary.user_expenses["User1"].size(), 2);
    EXPECT_EQ(summary.user_expenses["User2"].size(), 1);
    
    // Verify nested vector elements - check that both expenses are present (order may vary)
    const auto& user1_expenses = summary.user_expenses["User1"];
    bool has_exp1 = false, has_exp2 = false;
    for (const auto& exp : user1_expenses) {
        if (exp.title == "Exp1") has_exp1 = true;
        if (exp.title == "Exp2") has_exp2 = true;
    }
    EXPECT_TRUE(has_exp1);
    EXPECT_TRUE(has_exp2);
}

