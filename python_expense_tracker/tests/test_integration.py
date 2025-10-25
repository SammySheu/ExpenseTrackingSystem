"""
Integration tests for Expense Tracking System
Tests interactions between multiple modules and end-to-end workflows
"""

import pytest

import database
import models
import expense_operations


def test_complete_expense_workflow(clean_db):
    """Test complete workflow: create user → category → expense → view"""
    # Step 1: Create user
    user_id = models.create_user("IntegrationUser")
    assert user_id > 0, "User should be created"
    
    # Step 2: Create category
    category_id = models.create_category("IntegrationCategory")
    assert category_id > 0, "Category should be created"
    
    # Step 3: Create expense
    expense_id = models.insert_expense(
        date='2025-10-25',
        category_id=category_id,
        title='Integration Test Expense',
        amount=75.00,
        user_id=user_id
    )
    assert expense_id > 0, "Expense should be created"
    
    # Step 4: View expenses
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 1, "Should have one expense"
    assert expenses[0]['user_name'] == "IntegrationUser"
    assert expenses[0]['category_name'] == "IntegrationCategory"
    assert expenses[0]['title'] == 'Integration Test Expense'
    assert expenses[0]['amount'] == 75.00


def test_multiple_users_multiple_expenses(clean_db):
    """Test complex scenario with multiple users and expenses"""
    # Create multiple users
    user1_id = models.create_user("User1")
    user2_id = models.create_user("User2")
    user3_id = models.create_user("User3")
    
    # Get categories
    categories = models.get_all_categories()
    cat1 = categories[0]
    cat2 = categories[1]
    
    # Create expenses for different users
    models.insert_expense('2025-10-20', cat1['id'], 'Expense 1', 100.00, user1_id)
    models.insert_expense('2025-10-21', cat2['id'], 'Expense 2', 50.00, user1_id)
    models.insert_expense('2025-10-22', cat1['id'], 'Expense 3', 75.00, user2_id)
    models.insert_expense('2025-10-23', cat2['id'], 'Expense 4', 25.00, user2_id)
    models.insert_expense('2025-10-24', cat1['id'], 'Expense 5', 200.00, user3_id)
    
    # Test viewing all expenses
    all_expenses = expense_operations.view_all_expenses()
    assert len(all_expenses) == 5, "Should have 5 expenses"
    
    # Test filtering by user
    user1_expenses = expense_operations.view_expenses_by_user(user1_id)
    assert len(user1_expenses) == 2, "User1 should have 2 expenses"
    
    user2_expenses = expense_operations.view_expenses_by_user(user2_id)
    assert len(user2_expenses) == 2, "User2 should have 2 expenses"
    
    user3_expenses = expense_operations.view_expenses_by_user(user3_id)
    assert len(user3_expenses) == 1, "User3 should have 1 expense"
    
    # Test filtering by category
    cat1_expenses = expense_operations.view_expenses_by_category([cat1['name']])
    assert len(cat1_expenses) == 3, "Category 1 should have 3 expenses"
    
    # Test filtering by date range
    date_range_expenses = expense_operations.view_expenses_by_date('2025-10-21', '2025-10-23')
    assert len(date_range_expenses) == 3, "Should have 3 expenses in date range"


def test_filter_and_summary_workflow(clean_db, multiple_expenses):
    """Test filtering expenses and calculating summary"""
    # Filter by date range
    filtered_expenses = expense_operations.view_expenses_by_date('2025-10-21', '2025-10-23')
    
    # Calculate summary for filtered expenses
    summary = expense_operations.calculate_summary(filtered_expenses)
    
    assert summary['count'] == len(filtered_expenses), "Summary count should match filtered expenses"
    assert summary['total'] > 0, "Summary should have positive total"
    assert len(summary['by_category']) > 0, "Summary should have category breakdown"
    assert len(summary['by_user']) > 0, "Summary should have user breakdown"
    
    # Verify summary totals match
    user_total = sum(summary['by_user'].values())
    category_total = sum(summary['by_category'].values())
    
    assert abs(user_total - summary['total']) < 0.01, "User totals should match overall total"
    assert abs(category_total - summary['total']) < 0.01, "Category totals should match overall total"


def test_record_expense_with_auto_creation(clean_db):
    """Test record_expense function with automatic user and category creation"""
    # Record expense with new user and new category
    expense_id = expense_operations.record_expense(
        date='2025-10-25',
        category='AutoCategory',
        title='Auto Test',
        amount=99.99,
        user_name='AutoUser'
    )
    
    assert expense_id > 0, "Expense should be created"
    
    # Verify user was auto-created
    user = models.get_user_by_name('AutoUser')
    assert user is not None, "User should be auto-created"
    
    # Verify category was auto-created
    category = models.get_category_by_name('AutoCategory')
    assert category is not None, "Category should be auto-created"
    
    # Verify expense exists
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 1, "Should have one expense"
    assert expenses[0]['user_name'] == 'AutoUser'
    assert expenses[0]['category_name'] == 'AutoCategory'


def test_database_transaction_consistency(clean_db):
    """Test that database operations maintain consistency"""
    # Create user and category
    user_id = models.create_user("TransactionUser")
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    # Create multiple expenses (starting from 1 to avoid zero amount)
    for i in range(1, 11):
        models.insert_expense(
            date='2025-10-25',
            category_id=category_id,
            title=f'Expense {i}',
            amount=float(i * 10),
            user_id=user_id
        )
    
    # Verify all expenses were created
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 10, "All 10 expenses should be created"
    
    # Verify total is correct
    summary = expense_operations.calculate_summary()
    expected_total = sum(float(i * 10) for i in range(1, 11))
    assert abs(summary['total'] - expected_total) < 0.01, "Total should match expected"


def test_foreign_key_constraints(clean_db, sample_user):
    """Test that foreign key constraints are enforced"""
    # Try to create expense with invalid category_id
    with pytest.raises(Exception):  # Should raise IntegrityError
        models.insert_expense(
            date='2025-10-25',
            category_id=99999,
            title='Invalid Expense',
            amount=50.00,
            user_id=sample_user['id']
        )
    
    # Try to create expense with invalid user_id
    categories = models.get_all_categories()
    with pytest.raises(Exception):  # Should raise IntegrityError
        models.insert_expense(
            date='2025-10-25',
            category_id=categories[0]['id'],
            title='Invalid Expense',
            amount=50.00,
            user_id=99999
        )


def test_concurrent_expense_creation(clean_db):
    """Test creating multiple expenses concurrently"""
    # Create users
    user1_id = models.create_user("ConcurrentUser1")
    user2_id = models.create_user("ConcurrentUser2")
    
    # Get categories
    categories = models.get_all_categories()
    cat1 = categories[0]
    cat2 = categories[1]
    
    # Create expenses "concurrently" (sequentially in test)
    expense_ids = []
    expense_ids.append(models.insert_expense('2025-10-25', cat1['id'], 'Exp1', 10.00, user1_id))
    expense_ids.append(models.insert_expense('2025-10-25', cat2['id'], 'Exp2', 20.00, user2_id))
    expense_ids.append(models.insert_expense('2025-10-25', cat1['id'], 'Exp3', 30.00, user1_id))
    expense_ids.append(models.insert_expense('2025-10-25', cat2['id'], 'Exp4', 40.00, user2_id))
    
    # Verify all expenses have unique IDs
    assert len(set(expense_ids)) == 4, "All expense IDs should be unique"
    
    # Verify all expenses exist
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 4, "All 4 expenses should exist"


def test_data_integrity_across_operations(clean_db):
    """Test data integrity is maintained across multiple operations"""
    # Initial state
    initial_users = models.get_all_users()
    initial_categories = models.get_all_categories()
    initial_expenses = expense_operations.view_all_expenses()
    
    assert len(initial_users) == 0, "Should start with no users"
    assert len(initial_categories) >= 7, "Should have default categories"
    assert len(initial_expenses) == 0, "Should start with no expenses"
    
    # Add data
    user_id = models.create_user("IntegrityUser")
    category_id = initial_categories[0]['id']
    expense_id = models.insert_expense('2025-10-25', category_id, 'Test', 50.00, user_id)
    
    # Verify data
    users = models.get_all_users()
    expenses = expense_operations.view_all_expenses()
    
    assert len(users) == 1, "Should have one user"
    assert len(expenses) == 1, "Should have one expense"
    
    # Verify relationships
    expense = expenses[0]
    assert expense['user_name'] == "IntegrityUser", "Expense should link to correct user"
    assert expense['category_name'] == initial_categories[0]['name'], "Expense should link to correct category"


def test_filtering_combinations(clean_db, multiple_expenses):
    """Test various combinations of filters"""
    # Get test data
    users = models.get_all_users()
    categories = models.get_all_categories()
    
    # Test date + amount filters
    expenses1 = models.fetch_expenses_by_filters(
        min_date='2025-10-21',
        max_date='2025-10-23',
        min_amount=20.00,
        max_amount=50.00
    )
    
    for exp in expenses1:
        assert '2025-10-21' <= exp['date'] <= '2025-10-23'
        assert 20.00 <= exp['amount'] <= 50.00
    
    # Test date + category filters
    expenses2 = models.fetch_expenses_by_filters(
        min_date='2025-10-20',
        max_date='2025-10-24',
        category_ids=[categories[0]['id']]
    )
    
    for exp in expenses2:
        assert '2025-10-20' <= exp['date'] <= '2025-10-24'
        assert exp['category_name'] == categories[0]['name']
    
    # Test all filters combined
    expenses3 = models.fetch_expenses_by_filters(
        min_date='2025-10-20',
        max_date='2025-10-24',
        min_amount=20.00,
        max_amount=100.00,
        category_ids=[categories[0]['id']],
        user_id=users[0]['id']
    )
    
    for exp in expenses3:
        assert '2025-10-20' <= exp['date'] <= '2025-10-24'
        assert 20.00 <= exp['amount'] <= 100.00
        assert exp['category_name'] == categories[0]['name']
        assert exp['user_name'] == users[0]['name']


def test_summary_calculation_accuracy(clean_db):
    """Test that summary calculations are accurate"""
    # Create test data with known values
    user_id = models.create_user("SummaryUser")
    categories = models.get_all_categories()
    
    # Create expenses with specific amounts
    models.insert_expense('2025-10-25', categories[0]['id'], 'Exp1', 100.00, user_id)
    models.insert_expense('2025-10-25', categories[1]['id'], 'Exp2', 200.00, user_id)
    models.insert_expense('2025-10-25', categories[0]['id'], 'Exp3', 150.00, user_id)
    
    # Calculate summary
    summary = expense_operations.calculate_summary()
    
    # Verify totals
    assert summary['total'] == 450.00, "Total should be 450.00"
    assert summary['count'] == 3, "Count should be 3"
    
    # Verify category breakdown
    assert summary['by_category'][categories[0]['name']] == 250.00, "Category 1 should be 250.00"
    assert summary['by_category'][categories[1]['name']] == 200.00, "Category 2 should be 200.00"
    
    # Verify user breakdown
    assert summary['by_user']['SummaryUser'] == 450.00, "User total should be 450.00"

