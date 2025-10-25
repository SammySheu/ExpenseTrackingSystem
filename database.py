"""
Database layer for Expense Tracking System
Handles SQLite database initialization and connection management
"""

import sqlite3
import os
from datetime import datetime
from typing import Optional


def get_connection() -> sqlite3.Connection:
    """
    Create and return a database connection with foreign key support enabled
    
    Returns:
        sqlite3.Connection: Database connection with foreign keys enabled
    """
    conn = sqlite3.connect('expenses.db')
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def close_connection(conn: sqlite3.Connection) -> None:
    """
    Properly close database connection
    
    Args:
        conn: Database connection to close
    """
    if conn:
        conn.close()


def init_database() -> None:
    """
    Initialize the database with required tables and default categories
    Creates tables for Users, Categories, and Expenses with proper relationships
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        # Create Users table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS Users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL
            )
        ''')
        
        # Create Categories table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS Categories (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL
            )
        ''')
        
        # Create Expenses table with foreign key constraints
        cursor.execute('''
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
        ''')
        
        # Insert default categories if they don't exist
        default_categories = [
            'Food', 'Transportation', 'Entertainment', 
            'Utilities', 'Healthcare', 'Shopping', 'Other'
        ]
        
        for category in default_categories:
            cursor.execute('''
                INSERT OR IGNORE INTO Categories (name) VALUES (?)
            ''', (category,))
        
        conn.commit()
        print("Database initialized successfully with default categories.")
        
    except sqlite3.Error as e:
        print(f"Database initialization error: {e}")
        conn.rollback()
        raise
    finally:
        close_connection(conn)


def check_database_exists() -> bool:
    """
    Check if the database file exists
    
    Returns:
        bool: True if database file exists, False otherwise
    """
    return os.path.exists('expenses.db')


def get_current_timestamp() -> str:
    """
    Get current timestamp in ISO format for created_at fields
    
    Returns:
        str: Current timestamp in ISO format
    """
    return datetime.now().isoformat()
