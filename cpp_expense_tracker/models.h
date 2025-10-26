/**
 * models.h
 * Data access layer for Expense Tracking System
 * 
 * C++ FEATURES DEMONSTRATED:
 * - STL containers (vector, optional)
 * - Pass by reference for efficiency (const &)
 * - Static class methods
 * - Optional return types (C++17 feature)
 */

#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <optional>
#include "expense.h"
#include "database.h"

using namespace std;

/**
 * C++ FEATURE: Class with static methods
 * All database operations are organized as static methods
 * Similar to Python's module-level functions but with class organization
 */
class Models {
public:
    // User operations
    static int createUser(Database& db, const string& name);
    static vector<User> getAllUsers(Database& db);
    static optional<User> getUserByName(Database& db, const string& name);
    
    // Category operations
    static int createCategory(Database& db, const string& name);
    static vector<Category> getAllCategories(Database& db);
    static optional<Category> getCategoryByName(Database& db, const string& name);
    
    // Expense operations
    static int insertExpense(Database& db, const string& date, 
                           int category_id, const string& title, 
                           double amount, int user_id);
    
    /**
     * C++ FEATURE: Default parameter values
     * Optional parameters for filtering (similar to Python's None defaults)
     * Uses pointers for optional parameters
     */
    static vector<Expense> fetchExpensesByFilters(
        Database& db,
        const string* min_date = nullptr,
        const string* max_date = nullptr,
        const double* min_amount = nullptr,
        const double* max_amount = nullptr,
        const vector<int>* category_ids = nullptr,
        const int* user_id = nullptr
    );
};

#endif // MODELS_H

