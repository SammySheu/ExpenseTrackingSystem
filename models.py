"""
Data access layer for Expense Tracking System
Handles CRUD operations for Users, Categories, and Expenses
"""

import sqlite3
from typing import List, Dict, Optional, Tuple
from database import get_connection, close_connection, get_current_timestamp


def create_user(name: str) -> int:
    """
    Create a new user in the database
    
    Args:
        name: User's name (must be unique)
        
    Returns:
        int: User ID of the created user
        
    Raises:
        sqlite3.IntegrityError: If user name already exists
        ValueError: If name is empty or invalid
    """
    if not name or not name.strip():
        raise ValueError("User name cannot be empty")
    
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('INSERT INTO Users (name) VALUES (?)', (name.strip(),))
        user_id = cursor.lastrowid
        conn.commit()
        return user_id
    except sqlite3.IntegrityError:
        raise sqlite3.IntegrityError(f"User '{name}' already exists")
    finally:
        close_connection(conn)


def get_all_users() -> List[Dict[str, int]]:
    """
    Retrieve all users from the database
    
    Returns:
        List[Dict]: List of user dictionaries with id and name
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('SELECT id, name FROM Users ORDER BY name')
        users = [{'id': row[0], 'name': row[1]} for row in cursor.fetchall()]
        return users
    finally:
        close_connection(conn)


def get_user_by_name(name: str) -> Optional[Dict[str, int]]:
    """
    Get user by name
    
    Args:
        name: User's name to search for
        
    Returns:
        Optional[Dict]: User dictionary if found, None otherwise
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('SELECT id, name FROM Users WHERE name = ?', (name,))
        row = cursor.fetchone()
        return {'id': row[0], 'name': row[1]} if row else None
    finally:
        close_connection(conn)


def create_category(name: str) -> int:
    """
    Create a new category in the database
    
    Args:
        name: Category name (must be unique)
        
    Returns:
        int: Category ID of the created category
        
    Raises:
        sqlite3.IntegrityError: If category name already exists
        ValueError: If name is empty or invalid
    """
    if not name or not name.strip():
        raise ValueError("Category name cannot be empty")
    
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('INSERT INTO Categories (name) VALUES (?)', (name.strip(),))
        category_id = cursor.lastrowid
        conn.commit()
        return category_id
    except sqlite3.IntegrityError:
        raise sqlite3.IntegrityError(f"Category '{name}' already exists")
    finally:
        close_connection(conn)


def get_all_categories() -> List[Dict[str, int]]:
    """
    Retrieve all categories from the database
    
    Returns:
        List[Dict]: List of category dictionaries with id and name
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('SELECT id, name FROM Categories ORDER BY name')
        categories = [{'id': row[0], 'name': row[1]} for row in cursor.fetchall()]
        return categories
    finally:
        close_connection(conn)


def get_category_by_name(name: str) -> Optional[Dict[str, int]]:
    """
    Get category by name
    
    Args:
        name: Category name to search for
        
    Returns:
        Optional[Dict]: Category dictionary if found, None otherwise
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        cursor.execute('SELECT id, name FROM Categories WHERE name = ?', (name,))
        row = cursor.fetchone()
        return {'id': row[0], 'name': row[1]} if row else None
    finally:
        close_connection(conn)


def insert_expense(date: str, category_id: int, title: str, amount: float, user_id: int) -> int:
    """
    Insert a new expense record into the database
    
    Args:
        date: Expense date in YYYY-MM-DD format
        category_id: ID of the category
        title: Expense title/description
        amount: Expense amount
        user_id: ID of the user who made the expense
        
    Returns:
        int: Expense ID of the created expense
        
    Raises:
        sqlite3.IntegrityError: If foreign key constraints are violated
        ValueError: If required fields are empty or invalid
    """
    if not title or not title.strip():
        raise ValueError("Expense title cannot be empty")
    
    if amount <= 0:
        raise ValueError("Expense amount must be positive")
    
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        created_at = get_current_timestamp()
        cursor.execute('''
            INSERT INTO Expenses (date, category_id, title, amount, created_at, user_id)
            VALUES (?, ?, ?, ?, ?, ?)
        ''', (date, category_id, title.strip(), amount, created_at, user_id))
        
        expense_id = cursor.lastrowid
        conn.commit()
        return expense_id
    except sqlite3.IntegrityError as e:
        if "FOREIGN KEY constraint failed" in str(e):
            raise sqlite3.IntegrityError("Invalid category_id or user_id")
        raise
    finally:
        close_connection(conn)


def fetch_expenses_by_filters(
    min_date: Optional[str] = None,
    max_date: Optional[str] = None,
    min_amount: Optional[float] = None,
    max_amount: Optional[float] = None,
    category_ids: Optional[List[int]] = None,
    user_id: Optional[int] = None
) -> List[Dict]:
    """
    Generic function to fetch expenses with optional filters
    
    Args:
        min_date: Minimum date filter (YYYY-MM-DD)
        max_date: Maximum date filter (YYYY-MM-DD)
        min_amount: Minimum amount filter
        max_amount: Maximum amount filter
        category_ids: List of category IDs to filter by
        user_id: User ID to filter by
        
    Returns:
        List[Dict]: List of expense dictionaries with all fields
    """
    conn = get_connection()
    cursor = conn.cursor()
    
    try:
        # Build the base query with joins
        query = '''
            SELECT e.id, e.date, e.title, e.amount, e.created_at,
                   c.name as category_name, u.name as user_name
            FROM Expenses e
            JOIN Categories c ON e.category_id = c.id
            JOIN Users u ON e.user_id = u.id
        '''
        
        # Build WHERE clause conditions
        conditions = []
        params = []
        
        if min_date:
            conditions.append("e.date >= ?")
            params.append(min_date)
        
        if max_date:
            conditions.append("e.date <= ?")
            params.append(max_date)
        
        if min_amount is not None:
            conditions.append("e.amount >= ?")
            params.append(min_amount)
        
        if max_amount is not None:
            conditions.append("e.amount <= ?")
            params.append(max_amount)
        
        if category_ids:
            placeholders = ','.join(['?'] * len(category_ids))
            conditions.append(f"e.category_id IN ({placeholders})")
            params.extend(category_ids)
        
        if user_id is not None:
            conditions.append("e.user_id = ?")
            params.append(user_id)
        
        # Add WHERE clause if conditions exist
        if conditions:
            query += " WHERE " + " AND ".join(conditions)
        
        # Add ORDER BY clause
        query += " ORDER BY e.date DESC, e.created_at DESC"
        
        cursor.execute(query, params)
        rows = cursor.fetchall()
        
        # Convert to list of dictionaries
        expenses = []
        for row in rows:
            expense = {
                'id': row[0],
                'date': row[1],
                'title': row[2],
                'amount': row[3],
                'created_at': row[4],
                'category_name': row[5],
                'user_name': row[6]
            }
            expenses.append(expense)
        
        return expenses
    finally:
        close_connection(conn)
