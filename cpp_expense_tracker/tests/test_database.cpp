/**
 * test_database.cpp
 * Unit tests for Database class
 * 
 * C++ TESTING FEATURES DEMONSTRATED:
 * - RAII testing (constructor/destructor)
 * - Smart pointer testing
 * - File system operations
 * - Exception handling
 * - Static method testing
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "../database.h"
#include <fstream>

using namespace std;

/**
 * TEST: Database constructor opens connection
 * C++ FEATURE: RAII - constructor acquires resource
 */
TEST(DatabaseTest, DatabaseConstructorOpensConnection) {
    string path = "test_constructor.db";
    
    // C++ FEATURE: RAII - constructor creates and opens database
    Database db(path);
    
    // Verify connection is valid
    EXPECT_NE(db.getConnection(), nullptr);
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Database destructor closes connection
 * C++ FEATURE: RAII - destructor releases resource automatically
 * 
 * CONTRAST WITH PYTHON:
 * Python: Uses __exit__ in context managers
 * C++: Uses destructor called automatically when object goes out of scope
 */
TEST(DatabaseTest, DatabaseDestructorClosesConnection) {
    string path = "test_destructor.db";
    
    {
        // C++ FEATURE: Scope-based lifetime management
        Database db(path);
        EXPECT_NE(db.getConnection(), nullptr);
        // Destructor called here when db goes out of scope
    }
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Initialize database creates tables
 * Tests schema creation
 */
TEST(DatabaseTest, InitializeDatabaseCreatesTables) {
    string path = "test_init.db";
    Database db(path);
    
    // Initialize database
    EXPECT_TRUE(db.initialize());
    
    // Verify tables exist by querying sqlite_master
    sqlite3* conn = db.getConnection();
    sqlite3_stmt* stmt;
    
    const char* query = "SELECT name FROM sqlite_master WHERE type='table' "
                       "AND name IN ('Users', 'Categories', 'Expenses')";
    
    ASSERT_EQ(sqlite3_prepare_v2(conn, query, -1, &stmt, nullptr), SQLITE_OK);
    
    int table_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        table_count++;
    }
    
    sqlite3_finalize(stmt);
    
    // Should have 3 tables
    EXPECT_EQ(table_count, 3);
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Initialize database creates default categories
 */
TEST(DatabaseTest, InitializeDatabaseCreatesDefaultCategories) {
    string path = "test_categories.db";
    Database db(path);
    db.initialize();
    
    // Query categories
    sqlite3* conn = db.getConnection();
    sqlite3_stmt* stmt;
    
    const char* query = "SELECT COUNT(*) FROM Categories";
    ASSERT_EQ(sqlite3_prepare_v2(conn, query, -1, &stmt, nullptr), SQLITE_OK);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    
    // Should have default categories
    EXPECT_GT(count, 0);
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: GetConnection returns valid pointer
 * C++ FEATURE: Raw pointer testing
 */
TEST(DatabaseTest, GetConnectionReturnsValidPointer) {
    string path = "test_connection.db";
    Database db(path);
    
    // C++ FEATURE: Raw pointer from C API
    sqlite3* conn = db.getConnection();
    
    EXPECT_NE(conn, nullptr);
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Database::exists returns true for existing file
 * C++ FEATURE: Static method testing
 */
TEST(DatabaseTest, DatabaseExistsReturnsTrueForExistingFile) {
    string path = "test_exists.db";
    
    // Create file
    ofstream file(path);
    file.close();
    
    // C++ FEATURE: Static method call
    EXPECT_TRUE(Database::exists(path));
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Database::exists returns false for non-existent file
 */
TEST(DatabaseTest, DatabaseExistsReturnsFalseForNonExistent) {
    EXPECT_FALSE(Database::exists("nonexistent_file.db"));
}

/**
 * TEST: GetCurrentTimestamp returns valid format
 * Tests ISO 8601 timestamp format
 */
TEST(DatabaseTest, GetCurrentTimestampReturnsValidFormat) {
    // C++ FEATURE: Static method call
    string timestamp = Database::getCurrentTimestamp();
    
    // Verify format: YYYY-MM-DD HH:MM:SS
    EXPECT_EQ(timestamp.length(), 19);
    EXPECT_EQ(timestamp[4], '-');
    EXPECT_EQ(timestamp[7], '-');
    EXPECT_EQ(timestamp[10], ' ');
    EXPECT_EQ(timestamp[13], ':');
    EXPECT_EQ(timestamp[16], ':');
}

/**
 * TEST: Smart pointer cleanup
 * C++ FEATURE: unique_ptr for automatic memory management
 * 
 * CONTRAST WITH PYTHON:
 * Python: Garbage collector handles cleanup
 * C++: Smart pointers provide automatic cleanup
 */
TEST(DatabaseTest, SmartPointerCleanup) {
    string path = "test_smart_ptr.db";
    
    // C++ FEATURE: make_unique creates smart pointer
    auto db = make_unique<Database>(path);
    
    EXPECT_NE(db, nullptr);
    EXPECT_NE(db->getConnection(), nullptr);
    
    // db.reset() or going out of scope calls destructor automatically
    db.reset();
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: Multiple database instances
 * Tests that multiple Database objects can coexist
 */
TEST(DatabaseTest, MultipleDatabaseInstances) {
    string path1 = "test_multi1.db";
    string path2 = "test_multi2.db";
    
    Database db1(path1);
    Database db2(path2);
    
    db1.initialize();
    db2.initialize();
    
    EXPECT_NE(db1.getConnection(), nullptr);
    EXPECT_NE(db2.getConnection(), nullptr);
    EXPECT_NE(db1.getConnection(), db2.getConnection());
    
    // Cleanup
    remove(path1.c_str());
    remove(path2.c_str());
}

/**
 * TEST: Database handles foreign keys
 * Verifies PRAGMA foreign_keys is enabled
 */
TEST(DatabaseTest, DatabaseEnablesForeignKeys) {
    string path = "test_fk.db";
    Database db(path);
    db.initialize();
    
    sqlite3* conn = db.getConnection();
    sqlite3_stmt* stmt;
    
    const char* query = "PRAGMA foreign_keys";
    ASSERT_EQ(sqlite3_prepare_v2(conn, query, -1, &stmt, nullptr), SQLITE_OK);
    
    int fk_enabled = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fk_enabled = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    
    EXPECT_EQ(fk_enabled, 1);
    
    // Cleanup
    remove(path.c_str());
}

/**
 * TEST: RAII cleanup with exceptions
 * C++ FEATURE: Exception safety with RAII
 * 
 * Even if exception is thrown, destructor is still called
 */
TEST(DatabaseTest, RAIICleanupWithException) {
    string path = "test_exception.db";
    
    try {
        Database db(path);
        db.initialize();
        
        // Simulate exception
        throw runtime_error("Test exception");
        
    } catch (const exception& e) {
        // Exception caught, but destructor was still called
        EXPECT_STREQ(e.what(), "Test exception");
    }
    
    // Cleanup
    remove(path.c_str());
}

