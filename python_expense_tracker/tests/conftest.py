"""
Pytest fixtures and configuration for Expense Tracking System tests
Provides reusable test fixtures for database, users, categories, and expenses
"""

import pytest
import os
import sys
import sqlite3
import tempfile
from datetime import datetime

# Add parent directory to path to import modules
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import database
import models


@pytest.fixture
def temp_db(monkeypatch):
    """
    Create a temporary test database that is deleted after the test
    
    Yields:
        str: Path to temporary database file
    """
    # Create a temporary file
    db_fd, db_path = tempfile.mkstemp(suffix='.db')
    
    def temp_get_connection():
        conn = sqlite3.connect(db_path)
        conn.execute("PRAGMA foreign_keys = ON")
        return conn
    
    # Patch both database and models modules
    monkeypatch.setattr(database, 'get_connection', temp_get_connection)
    monkeypatch.setattr('models.get_connection', temp_get_connection)
    
    # Initialize the database tables manually
    conn = temp_get_connection()
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
        
        # Insert default categories
        default_categories = [
            'Food', 'Transportation', 'Entertainment', 
            'Utilities', 'Healthcare', 'Shopping', 'Other'
        ]
        
        for category in default_categories:
            cursor.execute('''
                INSERT OR IGNORE INTO Categories (name) VALUES (?)
            ''', (category,))
        
        conn.commit()
    finally:
        conn.close()
    
    yield db_path
    
    # Cleanup: remove temp file (monkeypatch will auto-restore)
    os.close(db_fd)
    os.unlink(db_path)


@pytest.fixture
def clean_db(temp_db):
    """
    Provide a clean database for each test
    Clears all data except default categories
    
    Args:
        temp_db: Temporary database fixture
        
    Yields:
        str: Path to clean database
    """
    # Just yield the temp_db - it's already initialized and clean
    yield temp_db
    
    # Cleanup after test (optional)
    conn = database.get_connection()
    cursor = conn.cursor()
    
    try:
        # Clear all expenses and users after test
        cursor.execute('DELETE FROM Expenses')
        cursor.execute('DELETE FROM Users')
        conn.commit()
    except:
        pass  # Ignore errors during cleanup
    finally:
        database.close_connection(conn)


@pytest.fixture
def sample_user(clean_db):
    """
    Create a sample user for testing
    
    Args:
        clean_db: Clean database fixture
        
    Returns:
        dict: User dictionary with id and name
    """
    user_id = models.create_user("TestUser")
    return {'id': user_id, 'name': "TestUser"}


@pytest.fixture
def sample_category(clean_db):
    """
    Create a sample category for testing
    
    Args:
        clean_db: Clean database fixture
        
    Returns:
        dict: Category dictionary with id and name
    """
    # Use existing default category
    categories = models.get_all_categories()
    return categories[0] if categories else None


@pytest.fixture
def sample_expense(clean_db, sample_user, sample_category):
    """
    Create a sample expense for testing
    
    Args:
        clean_db: Clean database fixture
        sample_user: Sample user fixture
        sample_category: Sample category fixture
        
    Returns:
        dict: Expense dictionary with all fields
    """
    expense_id = models.insert_expense(
        date='2025-10-25',
        category_id=sample_category['id'],
        title='Test Expense',
        amount=50.00,
        user_id=sample_user['id']
    )
    
    # Fetch and return the complete expense
    expenses = models.fetch_expenses_by_filters()
    return expenses[0] if expenses else None


@pytest.fixture
def multiple_expenses(clean_db, sample_user, sample_category):
    """
    Create multiple expenses for testing
    
    Args:
        clean_db: Clean database fixture
        sample_user: Sample user fixture
        sample_category: Sample category fixture
        
    Returns:
        list: List of expense dictionaries
    """
    # Create second user
    user2_id = models.create_user("TestUser2")
    
    # Get another category
    categories = models.get_all_categories()
    category2 = categories[1] if len(categories) > 1 else categories[0]
    
    # Create multiple expenses
    expenses_data = [
        ('2025-10-20', sample_category['id'], 'Lunch', 25.50, sample_user['id']),
        ('2025-10-21', category2['id'], 'Gas', 45.00, sample_user['id']),
        ('2025-10-22', sample_category['id'], 'Dinner', 60.00, user2_id),
        ('2025-10-23', category2['id'], 'Movie', 30.00, user2_id),
        ('2025-10-24', sample_category['id'], 'Groceries', 100.00, sample_user['id']),
    ]
    
    for date, cat_id, title, amount, user_id in expenses_data:
        models.insert_expense(date, cat_id, title, amount, user_id)
    
    # Fetch and return all expenses
    return models.fetch_expenses_by_filters()


@pytest.fixture
def mock_datetime(monkeypatch):
    """
    Mock datetime for consistent testing
    
    Args:
        monkeypatch: Pytest monkeypatch fixture
        
    Returns:
        datetime: Fixed datetime object
    """
    class MockDateTime:
        @staticmethod
        def now():
            return datetime(2025, 10, 25, 12, 0, 0)
        
        @staticmethod
        def isoformat():
            return '2025-10-25T12:00:00'
    
    monkeypatch.setattr(database, 'datetime', MockDateTime)
    return MockDateTime

