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

using namespace std;

class ExpenseOperations {
public:
    // Record a new expense with auto-creation of user/category if needed
    static int recordExpense(Database& db, const string& date,
                           const string& category, const string& title,
                           double amount, const string& user_name);
    
    // View expenses with various filters
    static vector<Expense> viewExpensesByDate(Database& db,
                                                   const string& min_date = "",
                                                   const string& max_date = "");
    
    static vector<Expense> viewExpensesByAmount(Database& db,
                                                     double min_amount = 0.0,
                                                     double max_amount = 0.0);
    
    static vector<Expense> viewExpensesByCategory(Database& db,
                                                       const vector<string>& categories);
    
    static vector<Expense> viewExpensesByUser(Database& db, int user_id);
    
    static vector<Expense> viewAllExpenses(Database& db);
    
    // Calculate summary statistics
    static ExpenseSummary calculateSummary(Database& db,
                                          const vector<Expense>* expenses = nullptr);
    
    // Interactive filter functions
    static vector<Expense> getExpensesByDateRange(Database& db);
    static vector<Expense> getExpensesByAmountRange(Database& db);
    static vector<Expense> getExpensesByCategory(Database& db);
    static vector<Expense> getExpensesByUser(Database& db);
};

#endif // EXPENSE_OPERATIONS_H

