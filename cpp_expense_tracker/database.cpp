/**
 * database.cpp
 * Implementation of database operations
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Manual memory management with SQLite C API
 * - RAII for automatic resource cleanup
 * - Error handling with return codes (vs Python exceptions)
 * - Pointer dereferencing and null checks
 */

#include "database.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

/**
 * C++ FEATURE: Constructor with initializer list
 * MEMORY MANAGEMENT: Opens database connection
 * Must be closed in destructor (RAII pattern)
 */
Database::Database(const string& path) : db(nullptr), db_path(path) {
    // C++ FEATURE: Manual error handling with return codes
    int rc = sqlite3_open(path.c_str(), &db);
    
    if (rc != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        db = nullptr;
    } else {
        // Enable foreign key constraints
        sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, nullptr);
    }
}

/**
 * C++ FEATURE: Destructor for automatic cleanup
 * MEMORY MANAGEMENT: Closes database connection automatically
 * when Database object goes out of scope
 * 
 * CONTRAST WITH PYTHON:
 * Python relies on garbage collection and context managers
 * C++ uses destructors for deterministic cleanup
 */
Database::~Database() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

sqlite3* Database::getConnection() {
    return db;
}

bool Database::exists(const string& path) {
    ifstream file(path);
    return file.good();
}

/**
 * C++ FEATURE: Using chrono library for date/time operations
 * CONTRAST WITH PYTHON: Python's datetime module is more straightforward
 * C++ requires more verbose code for date/time handling
 */
string Database::getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto time_t_now = chrono::system_clock::to_time_t(now);
    
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * Initialize database with schema and default data
 * 
 * C++ FEATURE: Manual SQL execution with error checking
 * CONTRAST WITH PYTHON: Python's sqlite3 module has similar API but with exceptions
 */
bool Database::initialize() {
    if (db == nullptr) {
        cerr << "Database not connected" << endl;
        return false;
    }
    
    char* errMsg = nullptr;
    
    // Create Users table
    const char* sql_users = R"(
        CREATE TABLE IF NOT EXISTS Users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL
        )
    )";
    
    if (sqlite3_exec(db, sql_users, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Users table: " << errMsg << endl;
        sqlite3_free(errMsg);  // C++ FEATURE: Manual memory cleanup
        return false;
    }
    
    // Create Categories table
    const char* sql_categories = R"(
        CREATE TABLE IF NOT EXISTS Categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL
        )
    )";
    
    if (sqlite3_exec(db, sql_categories, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Categories table: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    // Create Expenses table
    const char* sql_expenses = R"(
        CREATE TABLE IF NOT EXISTS Expenses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            category_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            amount REAL NOT NULL,
            created_at TEXT NOT NULL,
            user_id INTEGER NOT NULL,
            FOREIGN KEY (category_id) REFERENCES Categories (id),
            FOREIGN KEY (user_id) REFERENCES Users (id)
        )
    )";
    
    if (sqlite3_exec(db, sql_expenses, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Expenses table: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    // Insert default categories
    const char* default_categories[] = {
        "Food", "Transportation", "Entertainment", 
        "Utilities", "Healthcare", "Shopping", "Other"
    };
    
    // C++ FEATURE: Array size calculation at compile time
    int num_categories = sizeof(default_categories) / sizeof(default_categories[0]);
    
    for (int i = 0; i < num_categories; ++i) {
        string sql = "INSERT OR IGNORE INTO Categories (name) VALUES ('";
        sql += default_categories[i];
        sql += "')";
        
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "Error inserting category: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
    }
    
    cout << "Database initialized successfully with default categories." << endl;
    return true;
}

