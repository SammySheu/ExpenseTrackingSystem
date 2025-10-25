/**
 * models.cpp
 * Implementation of data access layer
 * 
 * C++ FEATURES DEMONSTRATED:
 * - SQLite prepared statements for security
 * - Manual memory management with SQLite handles
 * - STL container operations (push_back, etc.)
 * - Optional type for nullable returns
 */

#include "models.h"
#include <iostream>
#include <sstream>

/**
 * C++ FEATURE: Pass by reference to avoid copying
 * Database& db - reference avoids copying entire Database object
 * const std::string& name - const reference for read-only access
 * 
 * CONTRAST WITH PYTHON: Python always uses references implicitly
 */
int Models::createUser(Database& db, const std::string& name) {
    sqlite3* conn = db.getConnection();
    if (!conn) {
        throw std::runtime_error("Database not connected");
    }
    
    // C++ FEATURE: Prepared statements for SQL injection prevention
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Users (name) VALUES (?)";
    
    // Prepare statement
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }
    
    // Bind parameter
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    // Execute
    int rc = sqlite3_step(stmt);
    int user_id = 0;
    
    if (rc == SQLITE_DONE) {
        user_id = sqlite3_last_insert_rowid(conn);
    }
    
    // C++ FEATURE: Manual cleanup of prepared statement
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert user");
    }
    
    return user_id;
}

/**
 * C++ FEATURE: Returns STL vector container
 * CONTRAST WITH PYTHON: Python returns list of dictionaries
 * C++ returns vector of User structs with static types
 */
std::vector<User> Models::getAllUsers(Database& db) {
    std::vector<User> users;  // C++ FEATURE: STL vector container
    sqlite3* conn = db.getConnection();
    
    if (!conn) {
        return users;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name FROM Users ORDER BY name";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return users;
    }
    
    // C++ FEATURE: Manual iteration through result set
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name_text = sqlite3_column_text(stmt, 1);
        std::string name(reinterpret_cast<const char*>(name_text));
        
        // C++ FEATURE: Create struct and add to vector
        users.push_back(User(id, name));
    }
    
    sqlite3_finalize(stmt);
    return users;
}

/**
 * C++ FEATURE: std::optional for nullable return
 * Returns std::optional<User> - either contains a User or is empty
 * 
 * CONTRAST WITH PYTHON: Python returns None or a dict
 */
std::optional<User> Models::getUserByName(Database& db, const std::string& name) {
    sqlite3* conn = db.getConnection();
    if (!conn) {
        return std::nullopt;  // C++ FEATURE: std::nullopt represents "no value"
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name FROM Users WHERE name = ?";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    std::optional<User> result;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name_text = sqlite3_column_text(stmt, 1);
        std::string name_str(reinterpret_cast<const char*>(name_text));
        
        result = User(id, name_str);  // C++ FEATURE: Optional assignment
    }
    
    sqlite3_finalize(stmt);
    return result;
}

int Models::createCategory(Database& db, const std::string& name) {
    sqlite3* conn = db.getConnection();
    if (!conn) {
        throw std::runtime_error("Database not connected");
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Categories (name) VALUES (?)";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    int category_id = 0;
    
    if (rc == SQLITE_DONE) {
        category_id = sqlite3_last_insert_rowid(conn);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert category");
    }
    
    return category_id;
}

std::vector<Category> Models::getAllCategories(Database& db) {
    std::vector<Category> categories;
    sqlite3* conn = db.getConnection();
    
    if (!conn) {
        return categories;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name FROM Categories ORDER BY name";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return categories;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name_text = sqlite3_column_text(stmt, 1);
        std::string name(reinterpret_cast<const char*>(name_text));
        
        categories.push_back(Category(id, name));
    }
    
    sqlite3_finalize(stmt);
    return categories;
}

std::optional<Category> Models::getCategoryByName(Database& db, const std::string& name) {
    sqlite3* conn = db.getConnection();
    if (!conn) {
        return std::nullopt;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name FROM Categories WHERE name = ?";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    std::optional<Category> result;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name_text = sqlite3_column_text(stmt, 1);
        std::string name_str(reinterpret_cast<const char*>(name_text));
        
        result = Category(id, name_str);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

int Models::insertExpense(Database& db, const std::string& date, 
                         int category_id, const std::string& title, 
                         double amount, int user_id) {
    sqlite3* conn = db.getConnection();
    if (!conn) {
        throw std::runtime_error("Database not connected");
    }
    
    std::string created_at = Database::getCurrentTimestamp();
    
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO Expenses (date, category_id, title, amount, created_at, user_id)
        VALUES (?, ?, ?, ?, ?, ?)
    )";
    
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }
    
    // C++ FEATURE: Explicit type binding for each parameter
    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, category_id);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, amount);
    sqlite3_bind_text(stmt, 5, created_at.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, user_id);
    
    int rc = sqlite3_step(stmt);
    int expense_id = 0;
    
    if (rc == SQLITE_DONE) {
        expense_id = sqlite3_last_insert_rowid(conn);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert expense");
    }
    
    return expense_id;
}

/**
 * C++ FEATURE: Optional parameters using pointers
 * nullptr indicates "not provided" (similar to Python's None)
 * 
 * CONTRAST WITH PYTHON: Python uses Optional[] type hints and None
 * C++ uses raw pointers or std::optional
 */
std::vector<Expense> Models::fetchExpensesByFilters(
    Database& db,
    const std::string* min_date,
    const std::string* max_date,
    const double* min_amount,
    const double* max_amount,
    const std::vector<int>* category_ids,
    const int* user_id
) {
    std::vector<Expense> expenses;
    sqlite3* conn = db.getConnection();
    
    if (!conn) {
        return expenses;
    }
    
    // Build SQL query dynamically
    std::stringstream query;
    query << R"(
        SELECT e.id, e.date, e.title, e.amount, e.created_at,
               c.name as category_name, u.name as user_name
        FROM Expenses e
        JOIN Categories c ON e.category_id = c.id
        JOIN Users u ON e.user_id = u.id
    )";
    
    // Build WHERE clause
    std::vector<std::string> conditions;
    
    if (min_date != nullptr) {
        conditions.push_back("e.date >= ?");
    }
    if (max_date != nullptr) {
        conditions.push_back("e.date <= ?");
    }
    if (min_amount != nullptr) {
        conditions.push_back("e.amount >= ?");
    }
    if (max_amount != nullptr) {
        conditions.push_back("e.amount <= ?");
    }
    if (category_ids != nullptr && !category_ids->empty()) {
        std::stringstream cat_condition;
        cat_condition << "e.category_id IN (";
        for (size_t i = 0; i < category_ids->size(); ++i) {
            if (i > 0) cat_condition << ",";
            cat_condition << "?";
        }
        cat_condition << ")";
        conditions.push_back(cat_condition.str());
    }
    if (user_id != nullptr) {
        conditions.push_back("e.user_id = ?");
    }
    
    // Add WHERE clause if conditions exist
    if (!conditions.empty()) {
        query << " WHERE ";
        for (size_t i = 0; i < conditions.size(); ++i) {
            if (i > 0) query << " AND ";
            query << conditions[i];
        }
    }
    
    query << " ORDER BY e.date DESC, e.created_at DESC";
    
    // Prepare statement
    sqlite3_stmt* stmt;
    std::string sql_str = query.str();
    
    if (sqlite3_prepare_v2(conn, sql_str.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return expenses;
    }
    
    // Bind parameters
    int param_index = 1;
    
    if (min_date != nullptr) {
        sqlite3_bind_text(stmt, param_index++, min_date->c_str(), -1, SQLITE_TRANSIENT);
    }
    if (max_date != nullptr) {
        sqlite3_bind_text(stmt, param_index++, max_date->c_str(), -1, SQLITE_TRANSIENT);
    }
    if (min_amount != nullptr) {
        sqlite3_bind_double(stmt, param_index++, *min_amount);
    }
    if (max_amount != nullptr) {
        sqlite3_bind_double(stmt, param_index++, *max_amount);
    }
    if (category_ids != nullptr && !category_ids->empty()) {
        for (int cat_id : *category_ids) {
            sqlite3_bind_int(stmt, param_index++, cat_id);
        }
    }
    if (user_id != nullptr) {
        sqlite3_bind_int(stmt, param_index++, *user_id);
    }
    
    // Fetch results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Expense expense;
        expense.id = sqlite3_column_int(stmt, 0);
        expense.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        expense.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        expense.amount = sqlite3_column_double(stmt, 3);
        expense.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        expense.category_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        expense.user_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        
        expenses.push_back(expense);
    }
    
    sqlite3_finalize(stmt);
    return expenses;
}

