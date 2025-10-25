"""
Unit tests for database layer
Tests database initialization, connection management, and utility functions
"""

import pytest
import sqlite3
import os
from datetime import datetime

import database


def test_init_database_creates_tables(temp_db):
    """Test that init_database creates all required tables"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        # Check Users table exists
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='Users'")
        assert cursor.fetchone() is not None, "Users table should exist"
        
        # Check Categories table exists
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='Categories'")
        assert cursor.fetchone() is not None, "Categories table should exist"
        
        # Check Expenses table exists
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='Expenses'")
        assert cursor.fetchone() is not None, "Expenses table should exist"
    finally:
        database.close_connection(conn)


def test_init_database_creates_default_categories(temp_db):
    """Test that init_database creates default categories"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute("SELECT name FROM Categories ORDER BY name")
        categories = [row[0] for row in cursor.fetchall()]
        
        # Check that default categories exist
        expected_categories = ['Entertainment', 'Food', 'Healthcare', 'Other', 
                              'Shopping', 'Transportation', 'Utilities']
        assert categories == expected_categories, "Default categories should be created"
    finally:
        database.close_connection(conn)


def test_get_connection_returns_valid_connection(temp_db):
    """Test that get_connection returns a valid SQLite connection"""
    conn = database.get_connection()
    
    try:
        assert conn is not None, "Connection should not be None"
        assert isinstance(conn, sqlite3.Connection), "Should return sqlite3.Connection"
        
        # Test that connection works
        cursor = conn.cursor()
        cursor.execute("SELECT 1")
        result = cursor.fetchone()
        assert result[0] == 1, "Connection should be functional"
    finally:
        database.close_connection(conn)


def test_get_connection_enables_foreign_keys(temp_db):
    """Test that foreign key constraints are enabled"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute("PRAGMA foreign_keys")
        result = cursor.fetchone()
        assert result[0] == 1, "Foreign keys should be enabled"
    finally:
        database.close_connection(conn)


def test_close_connection(temp_db):
    """Test that close_connection properly closes the connection"""
    conn = database.get_connection()
    database.close_connection(conn)
    
    # Try to use closed connection - should raise error
    with pytest.raises(sqlite3.ProgrammingError):
        cursor = conn.cursor()
        cursor.execute("SELECT 1")


def test_close_connection_with_none():
    """Test that close_connection handles None gracefully"""
    # Should not raise any error
    database.close_connection(None)


def test_get_current_timestamp_format():
    """Test that get_current_timestamp returns ISO format"""
    timestamp = database.get_current_timestamp()
    
    # Check it's a string
    assert isinstance(timestamp, str), "Timestamp should be a string"
    
    # Check it can be parsed as ISO format
    try:
        dt = datetime.fromisoformat(timestamp)
        assert isinstance(dt, datetime), "Should be valid ISO format"
    except ValueError:
        pytest.fail("Timestamp should be in ISO format")


def test_check_database_exists_true(temp_db):
    """Test check_database_exists returns True when database exists"""
    # The temp_db fixture creates a database file
    # But check_database_exists checks for 'expenses.db'
    # So we need to create that file temporarily
    
    # Create a temporary expenses.db file
    with open('expenses.db', 'w') as f:
        f.write('')
    
    try:
        result = database.check_database_exists()
        assert result is True, "Should return True when database exists"
    finally:
        # Clean up
        if os.path.exists('expenses.db'):
            os.remove('expenses.db')


def test_check_database_exists_false():
    """Test check_database_exists returns False when database doesn't exist"""
    # Make sure expenses.db doesn't exist
    if os.path.exists('expenses.db'):
        os.remove('expenses.db')
    
    result = database.check_database_exists()
    assert result is False, "Should return False when database doesn't exist"


def test_database_schema_users_table(temp_db):
    """Test Users table has correct schema"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute("PRAGMA table_info(Users)")
        columns = {row[1]: row[2] for row in cursor.fetchall()}
        
        assert 'id' in columns, "Users table should have id column"
        assert 'name' in columns, "Users table should have name column"
        assert columns['name'] == 'TEXT', "name should be TEXT type"
    finally:
        database.close_connection(conn)


def test_database_schema_categories_table(temp_db):
    """Test Categories table has correct schema"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute("PRAGMA table_info(Categories)")
        columns = {row[1]: row[2] for row in cursor.fetchall()}
        
        assert 'id' in columns, "Categories table should have id column"
        assert 'name' in columns, "Categories table should have name column"
    finally:
        database.close_connection(conn)


def test_database_schema_expenses_table(temp_db):
    """Test Expenses table has correct schema"""
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute("PRAGMA table_info(Expenses)")
        columns = {row[1]: row[2] for row in cursor.fetchall()}
        
        assert 'id' in columns, "Expenses table should have id column"
        assert 'date' in columns, "Expenses table should have date column"
        assert 'category_id' in columns, "Expenses table should have category_id column"
        assert 'title' in columns, "Expenses table should have title column"
        assert 'amount' in columns, "Expenses table should have amount column"
        assert 'created_at' in columns, "Expenses table should have created_at column"
        assert 'user_id' in columns, "Expenses table should have user_id column"
    finally:
        database.close_connection(conn)

