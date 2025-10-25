/**
 * expense_operations.h
 * Business logic layer for Expense Tracking System
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Class-based organization vs Python's module functions
 * - STL containers for data management
 * - Exception handling (try/catch vs Python's try/except)
 */

#ifndef EXPENSE_OPERATIONS_H
#define EXPENSE_OPERATIONS_H

#include <vector>
#include <string>
#include "expense.h"
#include "database.h"

class ExpenseOperations {
public:
    // Record a new expense with auto-creation of user/category if needed
    static int recordExpense(Database& db, const std::string& date,
                           const std::string& category, const std::string& title,
                           double amount, const std::string& user_name);
    
    // View expenses with various filters
    static std::vector<Expense> viewExpensesByDate(Database& db,
                                                   const std::string& min_date = "",
                                                   const std::string& max_date = "");
    
    static std::vector<Expense> viewExpensesByAmount(Database& db,
                                                     double min_amount = 0.0,
                                                     double max_amount = 0.0);
    
    static std::vector<Expense> viewExpensesByCategory(Database& db,
                                                       const std::vector<std::string>& categories);
    
    static std::vector<Expense> viewExpensesByUser(Database& db, int user_id);
    
    static std::vector<Expense> viewAllExpenses(Database& db);
    
    // Calculate summary statistics
    static ExpenseSummary calculateSummary(Database& db,
                                          const std::vector<Expense>* expenses = nullptr);
    
    // Interactive filter functions
    static std::vector<Expense> getExpensesByDateRange(Database& db);
    static std::vector<Expense> getExpensesByAmountRange(Database& db);
    static std::vector<Expense> getExpensesByCategory(Database& db);
    static std::vector<Expense> getExpensesByUser(Database& db);
};

#endif // EXPENSE_OPERATIONS_H

