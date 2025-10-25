"""
Unit tests for utility functions
Tests validation and formatting functions
"""

import pytest
from datetime import datetime

import utils


# ============================================================================
# VALIDATION TESTS
# ============================================================================

def test_validate_date_valid_format():
    """Test validate_date accepts valid YYYY-MM-DD format"""
    result = utils.validate_date('2025-10-25')
    assert result == '2025-10-25', "Should return the validated date"


def test_validate_date_invalid_format_raises_error():
    """Test validate_date raises error for invalid format"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_date('10/25/2025')
    
    assert "YYYY-MM-DD" in str(exc_info.value), "Error should mention correct format"


def test_validate_date_invalid_date_raises_error():
    """Test validate_date raises error for invalid date like Feb 30"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_date('2025-02-30')
    
    assert "Invalid date" in str(exc_info.value)


def test_validate_date_empty_raises_error():
    """Test validate_date raises error for empty string"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_date('')
    
    assert "cannot be empty" in str(exc_info.value)


def test_validate_date_whitespace_raises_error():
    """Test validate_date raises error for whitespace"""
    with pytest.raises(ValueError):
        utils.validate_date('   ')


@pytest.mark.parametrize("invalid_date", [
    '2025-13-01',  # Invalid month
    '2025-00-01',  # Invalid month
    '2025-01-32',  # Invalid day
    '2025-01-00',  # Invalid day
    '25-10-2025',  # Wrong format
    '2025/10/25',  # Wrong separator
])
def test_validate_date_various_invalid_formats(invalid_date):
    """Test validate_date with various invalid formats"""
    with pytest.raises(ValueError):
        utils.validate_date(invalid_date)


def test_validate_amount_valid():
    """Test validate_amount accepts valid positive numbers"""
    result = utils.validate_amount('50.00')
    assert result == 50.00, "Should return float value"
    
    result = utils.validate_amount('100')
    assert result == 100.0, "Should handle integers"
    
    result = utils.validate_amount('0.01')
    assert result == 0.01, "Should handle small amounts"


def test_validate_amount_negative_raises_error():
    """Test validate_amount raises error for negative amounts"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_amount('-10.00')
    
    assert "must be positive" in str(exc_info.value)


def test_validate_amount_zero_raises_error():
    """Test validate_amount raises error for zero"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_amount('0')
    
    assert "must be positive" in str(exc_info.value)


def test_validate_amount_non_numeric_raises_error():
    """Test validate_amount raises error for non-numeric input"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_amount('abc')
    
    assert "valid number" in str(exc_info.value)


def test_validate_amount_empty_raises_error():
    """Test validate_amount raises error for empty string"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_amount('')
    
    assert "cannot be empty" in str(exc_info.value)


def test_validate_non_empty_valid():
    """Test validate_non_empty accepts non-empty strings"""
    result = utils.validate_non_empty('Test', 'Field')
    assert result == 'Test', "Should return stripped value"
    
    result = utils.validate_non_empty('  Test  ', 'Field')
    assert result == 'Test', "Should strip whitespace"


def test_validate_non_empty_raises_error():
    """Test validate_non_empty raises error for empty string"""
    with pytest.raises(ValueError) as exc_info:
        utils.validate_non_empty('', 'TestField')
    
    assert "TestField" in str(exc_info.value), "Error should mention field name"
    assert "cannot be empty" in str(exc_info.value)


def test_validate_non_empty_whitespace_raises_error():
    """Test validate_non_empty raises error for whitespace"""
    with pytest.raises(ValueError):
        utils.validate_non_empty('   ', 'Field')


# ============================================================================
# FORMATTING TESTS
# ============================================================================

def test_format_currency():
    """Test format_currency formats amounts correctly"""
    assert utils.format_currency(50.00) == '$50.00'
    assert utils.format_currency(100) == '$100.00'
    assert utils.format_currency(0.99) == '$0.99'
    assert utils.format_currency(1234.56) == '$1234.56'
    assert utils.format_currency(0.01) == '$0.01'


def test_format_currency_rounds_correctly():
    """Test format_currency rounds to 2 decimal places"""
    assert utils.format_currency(50.999) == '$51.00'
    assert utils.format_currency(50.001) == '$50.00'
    assert utils.format_currency(50.125) == '$50.12'


def test_format_expense_output_empty():
    """Test format_expense_output with empty list"""
    result = utils.format_expense_output([])
    assert result == "No expenses found."


def test_format_expense_output_single_expense():
    """Test format_expense_output with single expense"""
    expenses = [{
        'id': 1,
        'date': '2025-10-25',
        'title': 'Lunch',
        'amount': 25.50,
        'category_name': 'Food',
        'user_name': 'John'
    }]
    
    result = utils.format_expense_output(expenses)
    
    assert 'ID' in result, "Should have header"
    assert 'Date' in result, "Should have Date column"
    assert 'Title' in result, "Should have Title column"
    assert 'Amount' in result, "Should have Amount column"
    assert 'Category' in result, "Should have Category column"
    assert 'User' in result, "Should have User column"
    assert 'Lunch' in result, "Should contain expense title"
    assert '$25.50' in result, "Should contain formatted amount"
    assert 'Food' in result, "Should contain category"
    assert 'John' in result, "Should contain user"


def test_format_expense_output_multiple_expenses():
    """Test format_expense_output with multiple expenses"""
    expenses = [
        {
            'id': 1,
            'date': '2025-10-25',
            'title': 'Lunch',
            'amount': 25.50,
            'category_name': 'Food',
            'user_name': 'John'
        },
        {
            'id': 2,
            'date': '2025-10-24',
            'title': 'Gas',
            'amount': 45.00,
            'category_name': 'Transportation',
            'user_name': 'Jane'
        }
    ]
    
    result = utils.format_expense_output(expenses)
    
    # Check both expenses are present
    assert 'Lunch' in result
    assert 'Gas' in result
    assert '$25.50' in result
    assert '$45.00' in result
    
    # Check table structure
    lines = result.split('\n')
    assert len(lines) >= 4, "Should have header, separator, and expense rows"


def test_format_summary_output_empty():
    """Test format_summary_output with empty summary"""
    summary = {
        'total': 0.0,
        'count': 0,
        'by_category': {},
        'by_user': {},
        'user_expenses': {}
    }
    
    result = utils.format_summary_output(summary)
    
    assert 'EXPENSE SUMMARY' in result
    assert '$0.00' in result
    assert '0' in result


def test_format_summary_output_with_data():
    """Test format_summary_output with actual data"""
    summary = {
        'total': 150.00,
        'count': 3,
        'by_category': {
            'Food': 75.00,
            'Transportation': 75.00
        },
        'by_user': {
            'John': 100.00,
            'Jane': 50.00
        },
        'user_expenses': {
            'John': [
                {
                    'id': 1,
                    'date': '2025-10-25',
                    'title': 'Lunch',
                    'amount': 50.00,
                    'category_name': 'Food',
                    'user_name': 'John'
                },
                {
                    'id': 2,
                    'date': '2025-10-24',
                    'title': 'Gas',
                    'amount': 50.00,
                    'category_name': 'Transportation',
                    'user_name': 'John'
                }
            ],
            'Jane': [
                {
                    'id': 3,
                    'date': '2025-10-23',
                    'title': 'Groceries',
                    'amount': 50.00,
                    'category_name': 'Food',
                    'user_name': 'Jane'
                }
            ]
        }
    }
    
    result = utils.format_summary_output(summary)
    
    # Check all sections are present
    assert 'EXPENSE SUMMARY' in result
    assert 'EXPENSES BY USER' in result
    assert 'OVERALL SUMMARY' in result
    assert 'CATEGORY BREAKDOWN' in result
    
    # Check totals
    assert '$150.00' in result
    assert '3' in result or 'expense' in result.lower()
    
    # Check users
    assert 'John' in result
    assert 'Jane' in result


def test_format_summary_output_user_tables():
    """Test format_summary_output creates separate tables for each user"""
    summary = {
        'total': 100.00,
        'count': 2,
        'by_category': {'Food': 100.00},
        'by_user': {'John': 100.00},
        'user_expenses': {
            'John': [
                {
                    'id': 1,
                    'date': '2025-10-25',
                    'title': 'Lunch',
                    'amount': 50.00,
                    'category_name': 'Food',
                    'user_name': 'John'
                },
                {
                    'id': 2,
                    'date': '2025-10-24',
                    'title': 'Dinner',
                    'amount': 50.00,
                    'category_name': 'Food',
                    'user_name': 'John'
                }
            ]
        }
    }
    
    result = utils.format_summary_output(summary)
    
    # Check user's table is present
    assert "John's Expenses" in result
    assert 'Lunch' in result
    assert 'Dinner' in result
    assert 'TOTAL' in result


def test_format_summary_output_category_percentages():
    """Test format_summary_output calculates category percentages correctly"""
    summary = {
        'total': 100.00,
        'count': 2,
        'by_category': {
            'Food': 60.00,
            'Transportation': 40.00
        },
        'by_user': {'John': 100.00},
        'user_expenses': {
            'John': [
                {
                    'id': 1,
                    'date': '2025-10-25',
                    'title': 'Lunch',
                    'amount': 60.00,
                    'category_name': 'Food',
                    'user_name': 'John'
                },
                {
                    'id': 2,
                    'date': '2025-10-24',
                    'title': 'Gas',
                    'amount': 40.00,
                    'category_name': 'Transportation',
                    'user_name': 'John'
                }
            ]
        }
    }
    
    result = utils.format_summary_output(summary)
    
    # Check percentages are present
    assert '60.0%' in result or '60%' in result, "Should show 60% for Food"
    assert '40.0%' in result or '40%' in result, "Should show 40% for Transportation"


def test_get_menu_choice_valid(monkeypatch):
    """Test get_menu_choice with valid input"""
    monkeypatch.setattr('builtins.input', lambda _: '3')
    result = utils.get_menu_choice(5)
    assert result == 3


def test_get_menu_choice_zero(monkeypatch):
    """Test get_menu_choice accepts zero"""
    monkeypatch.setattr('builtins.input', lambda _: '0')
    result = utils.get_menu_choice(5)
    assert result == 0


def test_get_menu_choice_max(monkeypatch):
    """Test get_menu_choice accepts maximum value"""
    monkeypatch.setattr('builtins.input', lambda _: '5')
    result = utils.get_menu_choice(5)
    assert result == 5

