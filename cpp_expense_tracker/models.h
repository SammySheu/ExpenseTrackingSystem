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

/**
 * C++ FEATURE: Class with static methods
 * All database operations are organized as static methods
 * Similar to Python's module-level functions but with class organization
 */
class Models {
public:
    // User operations
    static int createUser(Database& db, const std::string& name);
    static std::vector<User> getAllUsers(Database& db);
    static std::optional<User> getUserByName(Database& db, const std::string& name);
    
    // Category operations
    static int createCategory(Database& db, const std::string& name);
    static std::vector<Category> getAllCategories(Database& db);
    static std::optional<Category> getCategoryByName(Database& db, const std::string& name);
    
    // Expense operations
    static int insertExpense(Database& db, const std::string& date, 
                           int category_id, const std::string& title, 
                           double amount, int user_id);
    
    /**
     * C++ FEATURE: Default parameter values
     * Optional parameters for filtering (similar to Python's None defaults)
     * Uses pointers for optional parameters
     */
    static std::vector<Expense> fetchExpensesByFilters(
        Database& db,
        const std::string* min_date = nullptr,
        const std::string* max_date = nullptr,
        const double* min_amount = nullptr,
        const double* max_amount = nullptr,
        const std::vector<int>* category_ids = nullptr,
        const int* user_id = nullptr
    );
};

#endif // MODELS_H

