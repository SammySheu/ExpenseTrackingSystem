"""
Unit tests for models (data access layer)
Tests CRUD operations for Users, Categories, and Expenses
"""

import pytest
import sqlite3

import models


# ============================================================================
# USER TESTS
# ============================================================================

def test_create_user_success(clean_db):
    """Test creating a user returns a valid user ID"""
    user_id = models.create_user("John Doe")
    
    assert user_id is not None, "Should return user ID"
    assert isinstance(user_id, int), "User ID should be an integer"
    assert user_id > 0, "User ID should be positive"


def test_create_user_duplicate_name_raises_error(clean_db, sample_user):
    """Test creating a user with duplicate name raises IntegrityError"""
    with pytest.raises(sqlite3.IntegrityError) as exc_info:
        models.create_user(sample_user['name'])
    
    assert "already exists" in str(exc_info.value), "Error message should mention duplicate"


def test_create_user_empty_name_raises_error(clean_db):
    """Test creating a user with empty name raises ValueError"""
    with pytest.raises(ValueError) as exc_info:
        models.create_user("")
    
    assert "cannot be empty" in str(exc_info.value)


def test_create_user_whitespace_name_raises_error(clean_db):
    """Test creating a user with only whitespace raises ValueError"""
    with pytest.raises(ValueError):
        models.create_user("   ")


def test_get_all_users_empty(clean_db):
    """Test get_all_users returns empty list when no users exist"""
    users = models.get_all_users()
    
    assert users == [], "Should return empty list"
    assert isinstance(users, list), "Should return a list"


def test_get_all_users_returns_all(clean_db):
    """Test get_all_users returns all users"""
    # Create multiple users
    models.create_user("Alice")
    models.create_user("Bob")
    models.create_user("Charlie")
    
    users = models.get_all_users()
    
    assert len(users) == 3, "Should return all 3 users"
    assert all('id' in user and 'name' in user for user in users), "Each user should have id and name"
    
    # Check they're sorted by name
    names = [user['name'] for user in users]
    assert names == ['Alice', 'Bob', 'Charlie'], "Users should be sorted by name"


def test_get_user_by_name_found(clean_db, sample_user):
    """Test get_user_by_name returns user when found"""
    user = models.get_user_by_name(sample_user['name'])
    
    assert user is not None, "Should find the user"
    assert user['id'] == sample_user['id'], "Should return correct user ID"
    assert user['name'] == sample_user['name'], "Should return correct user name"


def test_get_user_by_name_not_found(clean_db):
    """Test get_user_by_name returns None when user doesn't exist"""
    user = models.get_user_by_name("NonexistentUser")
    
    assert user is None, "Should return None for nonexistent user"


# ============================================================================
# CATEGORY TESTS
# ============================================================================

def test_create_category_success(clean_db):
    """Test creating a category returns a valid category ID"""
    category_id = models.create_category("NewCategory")
    
    assert category_id is not None, "Should return category ID"
    assert isinstance(category_id, int), "Category ID should be an integer"
    assert category_id > 0, "Category ID should be positive"


def test_create_category_duplicate_raises_error(clean_db):
    """Test creating a category with duplicate name raises IntegrityError"""
    models.create_category("TestCategory")
    
    with pytest.raises(sqlite3.IntegrityError) as exc_info:
        models.create_category("TestCategory")
    
    assert "already exists" in str(exc_info.value)


def test_create_category_empty_name_raises_error(clean_db):
    """Test creating a category with empty name raises ValueError"""
    with pytest.raises(ValueError) as exc_info:
        models.create_category("")
    
    assert "cannot be empty" in str(exc_info.value)


def test_get_all_categories_includes_defaults(clean_db):
    """Test get_all_categories includes default categories"""
    categories = models.get_all_categories()
    
    assert len(categories) >= 7, "Should have at least 7 default categories"
    
    # Check for some default categories
    category_names = [cat['name'] for cat in categories]
    assert 'Food' in category_names, "Should include Food category"
    assert 'Transportation' in category_names, "Should include Transportation category"
    assert 'Entertainment' in category_names, "Should include Entertainment category"


def test_get_category_by_name_found(clean_db, sample_category):
    """Test get_category_by_name returns category when found"""
    category = models.get_category_by_name(sample_category['name'])
    
    assert category is not None, "Should find the category"
    assert category['id'] == sample_category['id'], "Should return correct category ID"
    assert category['name'] == sample_category['name'], "Should return correct category name"


def test_get_category_by_name_not_found(clean_db):
    """Test get_category_by_name returns None when category doesn't exist"""
    category = models.get_category_by_name("NonexistentCategory")
    
    assert category is None, "Should return None for nonexistent category"


# ============================================================================
# EXPENSE TESTS
# ============================================================================

def test_insert_expense_success(clean_db, sample_user, sample_category):
    """Test inserting an expense returns a valid expense ID"""
    expense_id = models.insert_expense(
        date='2025-10-25',
        category_id=sample_category['id'],
        title='Test Expense',
        amount=50.00,
        user_id=sample_user['id']
    )
    
    assert expense_id is not None, "Should return expense ID"
    assert isinstance(expense_id, int), "Expense ID should be an integer"
    assert expense_id > 0, "Expense ID should be positive"


def test_insert_expense_invalid_category_id_raises_error(clean_db, sample_user):
    """Test inserting expense with invalid category_id raises IntegrityError"""
    with pytest.raises(sqlite3.IntegrityError) as exc_info:
        models.insert_expense(
            date='2025-10-25',
            category_id=99999,  # Invalid category ID
            title='Test Expense',
            amount=50.00,
            user_id=sample_user['id']
        )
    
    assert "Invalid category_id or user_id" in str(exc_info.value)


def test_insert_expense_invalid_user_id_raises_error(clean_db, sample_category):
    """Test inserting expense with invalid user_id raises IntegrityError"""
    with pytest.raises(sqlite3.IntegrityError) as exc_info:
        models.insert_expense(
            date='2025-10-25',
            category_id=sample_category['id'],
            title='Test Expense',
            amount=50.00,
            user_id=99999  # Invalid user ID
        )
    
    assert "Invalid category_id or user_id" in str(exc_info.value)


def test_insert_expense_empty_title_raises_error(clean_db, sample_user, sample_category):
    """Test inserting expense with empty title raises ValueError"""
    with pytest.raises(ValueError) as exc_info:
        models.insert_expense(
            date='2025-10-25',
            category_id=sample_category['id'],
            title='',
            amount=50.00,
            user_id=sample_user['id']
        )
    
    assert "cannot be empty" in str(exc_info.value)


def test_insert_expense_negative_amount_raises_error(clean_db, sample_user, sample_category):
    """Test inserting expense with negative amount raises ValueError"""
    with pytest.raises(ValueError) as exc_info:
        models.insert_expense(
            date='2025-10-25',
            category_id=sample_category['id'],
            title='Test Expense',
            amount=-10.00,
            user_id=sample_user['id']
        )
    
    assert "must be positive" in str(exc_info.value)


def test_insert_expense_zero_amount_raises_error(clean_db, sample_user, sample_category):
    """Test inserting expense with zero amount raises ValueError"""
    with pytest.raises(ValueError) as exc_info:
        models.insert_expense(
            date='2025-10-25',
            category_id=sample_category['id'],
            title='Test Expense',
            amount=0.00,
            user_id=sample_user['id']
        )
    
    assert "must be positive" in str(exc_info.value)


def test_fetch_expenses_by_filters_no_filters(clean_db, multiple_expenses):
    """Test fetching all expenses with no filters"""
    expenses = models.fetch_expenses_by_filters()
    
    assert len(expenses) == 5, "Should return all 5 expenses"
    assert all('id' in exp for exp in expenses), "Each expense should have id"
    assert all('date' in exp for exp in expenses), "Each expense should have date"
    assert all('title' in exp for exp in expenses), "Each expense should have title"
    assert all('amount' in exp for exp in expenses), "Each expense should have amount"
    assert all('category_name' in exp for exp in expenses), "Each expense should have category_name"
    assert all('user_name' in exp for exp in expenses), "Each expense should have user_name"


def test_fetch_expenses_by_filters_date_range(clean_db, multiple_expenses):
    """Test fetching expenses filtered by date range"""
    expenses = models.fetch_expenses_by_filters(min_date='2025-10-21', max_date='2025-10-23')
    
    assert len(expenses) == 3, "Should return 3 expenses in date range"
    
    # Verify all expenses are within range
    for expense in expenses:
        assert '2025-10-21' <= expense['date'] <= '2025-10-23', "Expense should be in date range"


def test_fetch_expenses_by_filters_amount_range(clean_db, multiple_expenses):
    """Test fetching expenses filtered by amount range"""
    expenses = models.fetch_expenses_by_filters(min_amount=30.00, max_amount=60.00)
    
    assert len(expenses) == 3, "Should return expenses in amount range"
    
    # Verify all expenses are within range
    for expense in expenses:
        assert 30.00 <= expense['amount'] <= 60.00, "Expense should be in amount range"


def test_fetch_expenses_by_filters_category_ids(clean_db, multiple_expenses, sample_category):
    """Test fetching expenses filtered by category IDs"""
    expenses = models.fetch_expenses_by_filters(category_ids=[sample_category['id']])
    
    assert len(expenses) >= 1, "Should return expenses for specified category"
    
    # Verify all expenses have the correct category
    for expense in expenses:
        assert expense['category_name'] == sample_category['name'], "Should match category"


def test_fetch_expenses_by_filters_user_id(clean_db, multiple_expenses, sample_user):
    """Test fetching expenses filtered by user ID"""
    expenses = models.fetch_expenses_by_filters(user_id=sample_user['id'])
    
    assert len(expenses) >= 1, "Should return expenses for specified user"
    
    # Verify all expenses belong to the user
    for expense in expenses:
        assert expense['user_name'] == sample_user['name'], "Should match user"


def test_fetch_expenses_by_filters_multiple_filters(clean_db, multiple_expenses, sample_user):
    """Test fetching expenses with multiple filters combined"""
    expenses = models.fetch_expenses_by_filters(
        min_date='2025-10-20',
        max_date='2025-10-24',
        min_amount=20.00,
        max_amount=50.00,
        user_id=sample_user['id']
    )
    
    # Should return expenses matching all criteria
    for expense in expenses:
        assert '2025-10-20' <= expense['date'] <= '2025-10-24', "Should be in date range"
        assert 20.00 <= expense['amount'] <= 50.00, "Should be in amount range"
        assert expense['user_name'] == sample_user['name'], "Should match user"


def test_fetch_expenses_empty_result(clean_db):
    """Test fetching expenses when no expenses exist"""
    expenses = models.fetch_expenses_by_filters()
    
    assert expenses == [], "Should return empty list"
    assert isinstance(expenses, list), "Should return a list"

