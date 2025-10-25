"""
Unit tests for expense operations (business logic)
Tests core functions for recording, viewing, and summarizing expenses
"""

import pytest
from io import StringIO
import sys

import expense_operations
import models


# ============================================================================
# RECORD EXPENSE TESTS
# ============================================================================

def test_record_expense_success(clean_db, sample_user, sample_category):
    """Test recording an expense successfully"""
    expense_id = expense_operations.record_expense(
        date='2025-10-25',
        category=sample_category['name'],
        title='Test Expense',
        amount=50.00,
        user_name=sample_user['name']
    )
    
    assert expense_id > 0, "Should return valid expense ID"
    
    # Verify expense was created
    expenses = models.fetch_expenses_by_filters()
    assert len(expenses) == 1, "Should have one expense"
    assert expenses[0]['title'] == 'Test Expense'


def test_record_expense_creates_new_user(clean_db, sample_category):
    """Test recording expense auto-creates new user"""
    expense_id = expense_operations.record_expense(
        date='2025-10-25',
        category=sample_category['name'],
        title='Test Expense',
        amount=50.00,
        user_name='NewUser'
    )
    
    assert expense_id > 0, "Should create expense"
    
    # Verify user was created
    user = models.get_user_by_name('NewUser')
    assert user is not None, "Should create new user"
    assert user['name'] == 'NewUser'


def test_record_expense_creates_new_category(clean_db, sample_user):
    """Test recording expense auto-creates new category"""
    expense_id = expense_operations.record_expense(
        date='2025-10-25',
        category='NewCategory',
        title='Test Expense',
        amount=50.00,
        user_name=sample_user['name']
    )
    
    assert expense_id > 0, "Should create expense"
    
    # Verify category was created
    category = models.get_category_by_name('NewCategory')
    assert category is not None, "Should create new category"
    assert category['name'] == 'NewCategory'


def test_record_expense_invalid_date_raises_error(clean_db, sample_user, sample_category):
    """Test recording expense with invalid date raises error"""
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='invalid-date',
            category=sample_category['name'],
            title='Test Expense',
            amount=50.00,
            user_name=sample_user['name']
        )


def test_record_expense_invalid_amount_raises_error(clean_db, sample_user, sample_category):
    """Test recording expense with invalid amount raises error"""
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=sample_category['name'],
            title='Test Expense',
            amount=-10.00,
            user_name=sample_user['name']
        )


def test_record_expense_empty_title_raises_error(clean_db, sample_user, sample_category):
    """Test recording expense with empty title raises error"""
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=sample_category['name'],
            title='',
            amount=50.00,
            user_name=sample_user['name']
        )


# ============================================================================
# VIEW EXPENSES TESTS
# ============================================================================

def test_view_expenses_by_date_no_filters(clean_db, multiple_expenses):
    """Test viewing all expenses with no date filters"""
    expenses = expense_operations.view_expenses_by_date()
    
    assert len(expenses) == 5, "Should return all expenses"


def test_view_expenses_by_date_min_only(clean_db, multiple_expenses):
    """Test viewing expenses with minimum date only"""
    expenses = expense_operations.view_expenses_by_date(min_date='2025-10-22')
    
    assert len(expenses) == 3, "Should return expenses from 10-22 onwards"
    for expense in expenses:
        assert expense['date'] >= '2025-10-22'


def test_view_expenses_by_date_max_only(clean_db, multiple_expenses):
    """Test viewing expenses with maximum date only"""
    expenses = expense_operations.view_expenses_by_date(max_date='2025-10-22')
    
    assert len(expenses) == 3, "Should return expenses up to 10-22"
    for expense in expenses:
        assert expense['date'] <= '2025-10-22'


def test_view_expenses_by_date_range(clean_db, multiple_expenses):
    """Test viewing expenses within date range"""
    expenses = expense_operations.view_expenses_by_date(
        min_date='2025-10-21',
        max_date='2025-10-23'
    )
    
    assert len(expenses) == 3, "Should return expenses in range"
    for expense in expenses:
        assert '2025-10-21' <= expense['date'] <= '2025-10-23'


def test_view_expenses_by_date_invalid_range_raises_error(clean_db):
    """Test viewing expenses with invalid date range"""
    expenses = expense_operations.view_expenses_by_date(
        min_date='2025-10-25',
        max_date='2025-10-20'
    )
    
    # Should return empty list due to invalid range
    assert expenses == []


def test_view_expenses_by_amount_range(clean_db, multiple_expenses):
    """Test viewing expenses filtered by amount range"""
    expenses = expense_operations.view_expenses_by_amount(
        min_amount=30.00,
        max_amount=60.00
    )
    
    assert len(expenses) >= 1, "Should return expenses in amount range"
    for expense in expenses:
        assert 30.00 <= expense['amount'] <= 60.00


def test_view_expenses_by_amount_negative_raises_error(clean_db):
    """Test viewing expenses with negative amount returns empty"""
    expenses = expense_operations.view_expenses_by_amount(min_amount=-10.00)
    
    # Should handle gracefully and return empty
    assert expenses == []


def test_view_expenses_by_category_single(clean_db, multiple_expenses, sample_category):
    """Test viewing expenses filtered by single category"""
    expenses = expense_operations.view_expenses_by_category([sample_category['name']])
    
    assert len(expenses) >= 1, "Should return expenses for category"
    for expense in expenses:
        assert expense['category_name'] == sample_category['name']


def test_view_expenses_by_category_multiple(clean_db, multiple_expenses):
    """Test viewing expenses filtered by multiple categories"""
    categories = models.get_all_categories()
    category_names = [cat['name'] for cat in categories[:2]]
    
    expenses = expense_operations.view_expenses_by_category(category_names)
    
    assert len(expenses) >= 1, "Should return expenses for specified categories"


def test_view_expenses_by_category_invalid_category(clean_db, multiple_expenses):
    """Test viewing expenses with invalid category name"""
    expenses = expense_operations.view_expenses_by_category(['NonexistentCategory'])
    
    assert expenses == [], "Should return empty list for invalid category"


def test_view_expenses_by_user_valid(clean_db, multiple_expenses, sample_user):
    """Test viewing expenses filtered by user ID"""
    expenses = expense_operations.view_expenses_by_user(sample_user['id'])
    
    assert len(expenses) >= 1, "Should return expenses for user"
    for expense in expenses:
        assert expense['user_name'] == sample_user['name']


def test_view_expenses_by_user_invalid_id_raises_error(clean_db):
    """Test viewing expenses with invalid user ID"""
    expenses = expense_operations.view_expenses_by_user(-1)
    
    # Should handle gracefully and return empty
    assert expenses == []


def test_view_all_expenses(clean_db, multiple_expenses):
    """Test viewing all expenses"""
    expenses = expense_operations.view_all_expenses()
    
    assert len(expenses) == 5, "Should return all 5 expenses"


def test_view_all_expenses_empty(clean_db):
    """Test viewing all expenses when none exist"""
    expenses = expense_operations.view_all_expenses()
    
    assert expenses == [], "Should return empty list"


# ============================================================================
# SUMMARY TESTS
# ============================================================================

def test_calculate_summary_empty(clean_db):
    """Test calculating summary with no expenses"""
    summary = expense_operations.calculate_summary()
    
    assert summary['total'] == 0.0, "Total should be zero"
    assert summary['count'] == 0, "Count should be zero"
    assert summary['by_category'] == {}, "Category breakdown should be empty"
    assert summary['by_user'] == {}, "User breakdown should be empty"


def test_calculate_summary_single_user(clean_db, sample_expense):
    """Test calculating summary with single user's expenses"""
    summary = expense_operations.calculate_summary()
    
    assert summary['total'] == 50.00, "Total should match expense amount"
    assert summary['count'] == 1, "Count should be 1"
    assert len(summary['by_user']) == 1, "Should have one user"
    assert 'TestUser' in summary['by_user'], "Should include test user"


def test_calculate_summary_multiple_users(clean_db, multiple_expenses):
    """Test calculating summary with multiple users"""
    summary = expense_operations.calculate_summary()
    
    assert summary['total'] > 0, "Should have positive total"
    assert summary['count'] == 5, "Should have 5 expenses"
    assert len(summary['by_user']) == 2, "Should have 2 users"
    assert 'TestUser' in summary['by_user']
    assert 'TestUser2' in summary['by_user']


def test_calculate_summary_by_category(clean_db, multiple_expenses):
    """Test calculating summary includes category totals"""
    summary = expense_operations.calculate_summary()
    
    assert len(summary['by_category']) >= 1, "Should have category breakdown"
    
    # Verify totals add up
    category_total = sum(summary['by_category'].values())
    assert abs(category_total - summary['total']) < 0.01, "Category totals should match overall total"


def test_calculate_summary_user_expenses_grouped(clean_db, multiple_expenses):
    """Test calculating summary groups expenses by user"""
    summary = expense_operations.calculate_summary()
    
    assert 'user_expenses' in summary, "Should have user_expenses field"
    assert len(summary['user_expenses']) == 2, "Should have expenses grouped by 2 users"
    
    # Verify each user has their expenses
    for user, expenses in summary['user_expenses'].items():
        assert isinstance(expenses, list), "User expenses should be a list"
        assert len(expenses) > 0, "Each user should have expenses"
        
        # Verify all expenses belong to the user
        for expense in expenses:
            assert expense['user_name'] == user, "Expense should belong to user"


def test_calculate_summary_with_specific_expenses(clean_db, multiple_expenses):
    """Test calculating summary with specific expense list"""
    # Get only expenses from one user
    all_expenses = models.fetch_expenses_by_filters()
    user_expenses = [exp for exp in all_expenses if exp['user_name'] == 'TestUser']
    
    summary = expense_operations.calculate_summary(user_expenses)
    
    assert summary['count'] == len(user_expenses), "Should match provided expenses"
    assert len(summary['by_user']) == 1, "Should only have one user"
    assert 'TestUser' in summary['by_user']


# ============================================================================
# INTERACTIVE FUNCTION TESTS (with mocking)
# ============================================================================

def test_get_expenses_by_date_range_with_dates(clean_db, multiple_expenses, monkeypatch):
    """Test interactive date range function with valid input"""
    # Mock user input
    inputs = iter(['2025-10-21', '2025-10-23'])
    monkeypatch.setattr('builtins.input', lambda _: next(inputs))
    
    expenses = expense_operations.get_expenses_by_date_range()
    
    assert len(expenses) == 3, "Should return expenses in date range"


def test_get_expenses_by_amount_range_with_amounts(clean_db, multiple_expenses, monkeypatch):
    """Test interactive amount range function with valid input"""
    # Mock user input
    inputs = iter(['30', '60'])
    monkeypatch.setattr('builtins.input', lambda _: next(inputs))
    
    expenses = expense_operations.get_expenses_by_amount_range()
    
    assert len(expenses) >= 1, "Should return expenses in amount range"


def test_get_expenses_by_category_interactive_with_selection(clean_db, multiple_expenses, monkeypatch):
    """Test interactive category selection"""
    # Mock user input: select category 1, then done
    inputs = iter(['1', 'done'])
    monkeypatch.setattr('builtins.input', lambda _: next(inputs))
    
    expenses = expense_operations.get_expenses_by_category_interactive()
    
    assert len(expenses) >= 1, "Should return expenses for selected category"


def test_get_expenses_by_user_interactive_with_selection(clean_db, multiple_expenses, monkeypatch):
    """Test interactive user selection"""
    # Mock user input: select user 1
    monkeypatch.setattr('builtins.input', lambda _: '1')
    
    expenses = expense_operations.get_expenses_by_user_interactive()
    
    assert len(expenses) >= 1, "Should return expenses for selected user"

