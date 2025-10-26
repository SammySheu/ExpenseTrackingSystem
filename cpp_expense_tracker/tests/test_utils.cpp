/**
 * test_utils.cpp
 * Unit tests for Utils class
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - String manipulation testing
 * - Reference parameter testing
 * - Boolean return value testing
 * - String formatting testing
 * - STL algorithm testing
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../utils.h"

using namespace std;

// ============================================================================
// VALIDATION TESTS
// ============================================================================

/**
 * TEST: ValidateDate accepts valid format
 * C++ FEATURE: String parameter testing
 */
TEST(UtilsTest, ValidateDateAcceptsValidFormat) {
    // C++ FEATURE: Static method call with const string reference
    EXPECT_TRUE(Utils::validateDate("2025-10-25"));
    EXPECT_TRUE(Utils::validateDate("2024-01-01"));
    EXPECT_TRUE(Utils::validateDate("2023-12-31"));
}

/**
 * TEST: ValidateDate rejects invalid format
 * Tests various invalid date formats
 */
TEST(UtilsTest, ValidateDateRejectsInvalidFormat) {
    EXPECT_FALSE(Utils::validateDate("25-10-2025"));  // Wrong order
    EXPECT_FALSE(Utils::validateDate("2025/10/25"));  // Wrong separator
    EXPECT_FALSE(Utils::validateDate("2025-10-5"));   // Missing zero
    EXPECT_FALSE(Utils::validateDate("2025-1-25"));   // Missing zero
    EXPECT_FALSE(Utils::validateDate("25-10-25"));    // Short year
    EXPECT_FALSE(Utils::validateDate(""));            // Empty
    EXPECT_FALSE(Utils::validateDate("not-a-date"));  // Invalid
}

/**
 * TEST: ValidateDate rejects invalid dates
 * Tests logical date validation (e.g., Feb 30)
 */
TEST(UtilsTest, ValidateDateRejectsInvalidDate) {
    EXPECT_FALSE(Utils::validateDate("2025-02-30"));  // Feb doesn't have 30 days
    EXPECT_FALSE(Utils::validateDate("2025-13-01"));  // Month > 12
    EXPECT_FALSE(Utils::validateDate("2025-00-01"));  // Month < 1
    EXPECT_FALSE(Utils::validateDate("2025-10-32"));  // Day > 31
    EXPECT_FALSE(Utils::validateDate("2025-10-00"));  // Day < 1
}

/**
 * TEST: ValidateAmount accepts positive values
 * C++ FEATURE: Double parameter testing
 */
TEST(UtilsTest, ValidateAmountAcceptsPositive) {
    EXPECT_TRUE(Utils::validateAmount(0.01));
    EXPECT_TRUE(Utils::validateAmount(1.0));
    EXPECT_TRUE(Utils::validateAmount(100.50));
    EXPECT_TRUE(Utils::validateAmount(9999.99));
}

/**
 * TEST: ValidateAmount rejects negative values
 */
TEST(UtilsTest, ValidateAmountRejectsNegative) {
    EXPECT_FALSE(Utils::validateAmount(-0.01));
    EXPECT_FALSE(Utils::validateAmount(-1.0));
    EXPECT_FALSE(Utils::validateAmount(-100.0));
}

/**
 * TEST: ValidateAmount rejects zero
 */
TEST(UtilsTest, ValidateAmountRejectsZero) {
    EXPECT_FALSE(Utils::validateAmount(0.0));
}

/**
 * TEST: ValidateNonEmpty accepts valid strings
 */
TEST(UtilsTest, ValidateNonEmptyAcceptsValidString) {
    EXPECT_TRUE(Utils::validateNonEmpty("Valid"));
    EXPECT_TRUE(Utils::validateNonEmpty("Multiple Words"));
    EXPECT_TRUE(Utils::validateNonEmpty("123"));
}

/**
 * TEST: ValidateNonEmpty rejects empty strings
 * C++ FEATURE: String validation
 */
TEST(UtilsTest, ValidateNonEmptyRejectsEmpty) {
    EXPECT_FALSE(Utils::validateNonEmpty(""));
    EXPECT_FALSE(Utils::validateNonEmpty("   "));      // Whitespace only
    EXPECT_FALSE(Utils::validateNonEmpty("\t\n"));     // Tabs and newlines
}

// ============================================================================
// PARSING TESTS
// ============================================================================

/**
 * TEST: ParseDate succeeds with valid input
 * C++ FEATURE: Reference parameter for output
 * 
 * CONTRAST WITH PYTHON:
 * Python: Returns tuple or uses return value
 * C++: Uses reference parameter for output, returns bool for success
 */
TEST(UtilsTest, ParseDateSucceedsWithValidInput) {
    string output;
    
    // C++ FEATURE: Reference parameter modified by function
    EXPECT_TRUE(Utils::parseDate("2025-10-25", output));
    EXPECT_EQ(output, "2025-10-25");
    
    EXPECT_TRUE(Utils::parseDate("  2025-10-25  ", output));
    EXPECT_EQ(output, "2025-10-25");  // Trimmed
}

/**
 * TEST: ParseDate fails with invalid input
 */
TEST(UtilsTest, ParseDateFailsWithInvalidInput) {
    string output;
    
    EXPECT_FALSE(Utils::parseDate("invalid", output));
    EXPECT_FALSE(Utils::parseDate("", output));
    EXPECT_FALSE(Utils::parseDate("2025/10/25", output));
}

/**
 * TEST: ParseAmount succeeds with valid input
 * C++ FEATURE: Reference parameter for double output
 */
TEST(UtilsTest, ParseAmountSucceedsWithValidInput) {
    double output;
    
    EXPECT_TRUE(Utils::parseAmount("50.00", output));
    EXPECT_DOUBLE_EQ(output, 50.0);
    
    EXPECT_TRUE(Utils::parseAmount("100", output));
    EXPECT_DOUBLE_EQ(output, 100.0);
    
    EXPECT_TRUE(Utils::parseAmount("  25.50  ", output));
    EXPECT_DOUBLE_EQ(output, 25.50);
}

/**
 * TEST: ParseAmount fails with invalid input
 */
TEST(UtilsTest, ParseAmountFailsWithInvalidInput) {
    double output;
    
    EXPECT_FALSE(Utils::parseAmount("invalid", output));
    EXPECT_FALSE(Utils::parseAmount("", output));
    EXPECT_FALSE(Utils::parseAmount("-50", output));  // Negative
    EXPECT_FALSE(Utils::parseAmount("0", output));    // Zero
}

// ============================================================================
// FORMATTING TESTS
// ============================================================================

/**
 * TEST: FormatCurrency formats correctly
 * C++ FEATURE: String formatting with iomanip
 * 
 * CONTRAST WITH PYTHON:
 * Python: Uses f-strings or format()
 * C++: Uses ostringstream with iomanip
 */
TEST(UtilsTest, FormatCurrencyFormatsCorrectly) {
    EXPECT_EQ(Utils::formatCurrency(50.0), "$50.00");
    EXPECT_EQ(Utils::formatCurrency(100.50), "$100.50");
    EXPECT_EQ(Utils::formatCurrency(0.99), "$0.99");
    EXPECT_EQ(Utils::formatCurrency(1000.0), "$1000.00");
}

/**
 * TEST: FormatExpenseOutput handles empty vector
 * C++ FEATURE: STL vector parameter
 */
TEST(UtilsTest, FormatExpenseOutputHandlesEmpty) {
    // C++ FEATURE: Empty vector initialization
    vector<Expense> expenses;
    
    string output = Utils::formatExpenseOutput(expenses);
    
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("No expenses"), string::npos);
}

/**
 * TEST: FormatExpenseOutput single expense
 * Tests table formatting for one expense
 */
TEST_F(ModelsTest, FormatExpenseOutputSingleExpense) {
    int user_id = createSampleUser("TestUser");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Test Expense", 50.0, user_id);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    string output = Utils::formatExpenseOutput(expenses);
    
    // Verify output contains expense details
    EXPECT_NE(output.find("Test Expense"), string::npos);
    EXPECT_NE(output.find("$50.00"), string::npos);
    EXPECT_NE(output.find("2025-10-25"), string::npos);
    EXPECT_NE(output.find("TestUser"), string::npos);
}

/**
 * TEST: FormatExpenseOutput multiple expenses
 * Tests table formatting and alignment
 */
TEST_F(ModelsTest, FormatExpenseOutputMultipleExpenses) {
    int user_id = createSampleUser();
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Expense 1", 10.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Expense 2", 20.0, user_id);
    Models::insertExpense(*db, "2025-10-27", category_id, "Expense 3", 30.0, user_id);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    string output = Utils::formatExpenseOutput(expenses);
    
    // Verify all expenses are in output
    EXPECT_NE(output.find("Expense 1"), string::npos);
    EXPECT_NE(output.find("Expense 2"), string::npos);
    EXPECT_NE(output.find("Expense 3"), string::npos);
    
    // Verify table structure
    EXPECT_NE(output.find("Date"), string::npos);
    EXPECT_NE(output.find("Title"), string::npos);
    EXPECT_NE(output.find("Amount"), string::npos);
}

/**
 * TEST: FormatSummaryOutput with empty data
 * C++ FEATURE: Struct parameter
 */
TEST(UtilsTest, FormatSummaryOutputEmpty) {
    // C++ FEATURE: Struct initialization
    ExpenseSummary summary;
    summary.total = 0.0;
    summary.count = 0;
    
    string output = Utils::formatSummaryOutput(summary);
    
    EXPECT_NE(output.find("$0.00"), string::npos);
    EXPECT_NE(output.find("0"), string::npos);
}

/**
 * TEST: FormatSummaryOutput with data
 * Tests all sections of summary output
 */
TEST_F(ModelsTest, FormatSummaryOutputWithData) {
    int user_id = createSampleUser("TestUser");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "Expense 1", 50.0, user_id);
    Models::insertExpense(*db, "2025-10-26", category_id, "Expense 2", 30.0, user_id);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    string output = Utils::formatSummaryOutput(summary);
    
    // Verify summary sections
    EXPECT_NE(output.find("EXPENSE SUMMARY"), string::npos);
    EXPECT_NE(output.find("Total Expenses"), string::npos);
    EXPECT_NE(output.find("$80.00"), string::npos);
}

/**
 * TEST: FormatSummaryOutput user tables
 * Tests user-specific expense tables
 */
TEST_F(ModelsTest, FormatSummaryOutputUserTables) {
    int user1 = Models::createUser(*db, "User1");
    int user2 = Models::createUser(*db, "User2");
    int category_id = createSampleCategory();
    
    Models::insertExpense(*db, "2025-10-25", category_id, "User1 Exp", 50.0, user1);
    Models::insertExpense(*db, "2025-10-26", category_id, "User2 Exp", 30.0, user2);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    string output = Utils::formatSummaryOutput(summary);
    
    // Verify user sections
    EXPECT_NE(output.find("User1"), string::npos);
    EXPECT_NE(output.find("User2"), string::npos);
    EXPECT_NE(output.find("EXPENSES BY USER"), string::npos);
}

/**
 * TEST: FormatSummaryOutput category percentages
 * Tests percentage calculations in output
 */
TEST_F(ModelsTest, FormatSummaryOutputCategoryPercentages) {
    int user_id = createSampleUser();
    auto categories = Models::getAllCategories(*db);
    ASSERT_GE(categories.size(), 2);
    
    // 75% in first category, 25% in second
    Models::insertExpense(*db, "2025-10-25", categories[0].id, "Exp1", 75.0, user_id);
    Models::insertExpense(*db, "2025-10-26", categories[1].id, "Exp2", 25.0, user_id);
    
    vector<Expense> expenses = Models::fetchExpensesByFilters(*db);
    ExpenseSummary summary = ExpenseOperations::calculateSummary(*db, &expenses);
    
    string output = Utils::formatSummaryOutput(summary);
    
    // Verify percentages are shown
    EXPECT_NE(output.find("%"), string::npos);
    EXPECT_NE(output.find("CATEGORY BREAKDOWN"), string::npos);
}

// ============================================================================
// STRING UTILITY TESTS
// ============================================================================

/**
 * TEST: Trim removes whitespace
 * C++ FEATURE: String manipulation
 * 
 * CONTRAST WITH PYTHON:
 * Python: str.strip()
 * C++: Custom trim function or string manipulation
 */
TEST(UtilsTest, TrimRemovesWhitespace) {
    EXPECT_EQ(Utils::trim("  test  "), "test");
    EXPECT_EQ(Utils::trim("test"), "test");
    EXPECT_EQ(Utils::trim("  test"), "test");
    EXPECT_EQ(Utils::trim("test  "), "test");
    EXPECT_EQ(Utils::trim("\t\ntest\t\n"), "test");
}

/**
 * TEST: ToLower converts correctly
 * C++ FEATURE: String transformation
 * 
 * CONTRAST WITH PYTHON:
 * Python: str.lower()
 * C++: transform with tolower
 */
TEST(UtilsTest, ToLowerConvertsCorrectly) {
    EXPECT_EQ(Utils::toLower("TEST"), "test");
    EXPECT_EQ(Utils::toLower("Test"), "test");
    EXPECT_EQ(Utils::toLower("TeSt"), "test");
    EXPECT_EQ(Utils::toLower("test"), "test");
    EXPECT_EQ(Utils::toLower("TEST123"), "test123");
}

