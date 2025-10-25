/**
 * database.h
 * Database layer for Expense Tracking System
 * 
 * C++ FEATURES DEMONSTRATED:
 * - RAII pattern for resource management (Database class)
 * - Manual memory management with SQLite C API
 * - Pointer usage for database connections
 * - Destructor for cleanup (vs Python's garbage collection)
 * - Class-based encapsulation
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>

/**
 * C++ FEATURE: RAII (Resource Acquisition Is Initialization)
 * Database class manages SQLite connection lifetime
 * Constructor opens connection, destructor closes it
 * 
 * CONTRAST WITH PYTHON:
 * Python uses context managers (with statements) for resource management
 * C++ uses RAII and destructors for automatic cleanup
 */
class Database {
private:
    sqlite3* db;  // C++ FEATURE: Raw pointer for C API interaction
    std::string db_path;
    
public:
    // Constructor - opens database connection
    Database(const std::string& path);
    
    // C++ FEATURE: Destructor for automatic resource cleanup
    // Called automatically when object goes out of scope
    ~Database();
    
    // Prevent copying (C++ FEATURE: deleted copy constructor)
    // This ensures only one Database object manages the connection
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    // Initialize database schema
    bool initialize();
    
    // Get raw database pointer for queries
    sqlite3* getConnection();
    
    // Check if database file exists
    static bool exists(const std::string& path);
    
    // Get current timestamp
    static std::string getCurrentTimestamp();
};

#endif // DATABASE_H

