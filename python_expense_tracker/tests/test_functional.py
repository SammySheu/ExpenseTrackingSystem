"""
Functional tests for Expense Tracking System
Tests user stories and end-to-end functional scenarios
"""

import pytest

import models
import expense_operations
import utils


def test_user_can_record_expense(clean_db):
    """
    User Story: As a user, I want to record an expense
    so that I can track my spending
    """
    # Given: A user wants to record an expense
    user_name = "Alice"
    category = "Food"
    title = "Lunch at restaurant"
    amount = 45.50
    date = "2025-10-25"
    
    # When: The user records the expense
    expense_id = expense_operations.record_expense(
        date=date,
        category=category,
        title=title,
        amount=amount,
        user_name=user_name
    )
    
    # Then: The expense should be successfully recorded
    assert expense_id > 0, "Expense should be recorded with valid ID"
    
    # And: The expense should be retrievable
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 1, "Should have one expense"
    
    recorded_expense = expenses[0]
    assert recorded_expense['user_name'] == user_name
    assert recorded_expense['category_name'] == category
    assert recorded_expense['title'] == title
    assert recorded_expense['amount'] == amount
    assert recorded_expense['date'] == date


def test_user_can_view_expenses_by_date(clean_db):
    """
    User Story: As a user, I want to view expenses within a date range
    so that I can see my spending for a specific period
    """
    # Given: Multiple expenses exist across different dates
    user_id = models.create_user("Bob")
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    models.insert_expense('2025-10-20', category_id, 'Expense 1', 100.00, user_id)
    models.insert_expense('2025-10-22', category_id, 'Expense 2', 50.00, user_id)
    models.insert_expense('2025-10-24', category_id, 'Expense 3', 75.00, user_id)
    models.insert_expense('2025-10-26', category_id, 'Expense 4', 25.00, user_id)
    
    # When: The user filters expenses by date range
    filtered_expenses = expense_operations.view_expenses_by_date(
        min_date='2025-10-21',
        max_date='2025-10-25'
    )
    
    # Then: Only expenses within the date range should be returned
    assert len(filtered_expenses) == 2, "Should return 2 expenses in date range"
    
    for expense in filtered_expenses:
        assert '2025-10-21' <= expense['date'] <= '2025-10-25', \
            "All expenses should be within date range"


def test_user_can_view_summary(clean_db):
    """
    User Story: As a user, I want to view a summary of my expenses
    so that I can understand my spending patterns
    """
    # Given: Multiple users have recorded expenses in different categories
    user1_id = models.create_user("Charlie")
    user2_id = models.create_user("Diana")
    
    categories = models.get_all_categories()
    food_cat = categories[0]
    transport_cat = categories[1]
    
    # Charlie's expenses
    models.insert_expense('2025-10-25', food_cat['id'], 'Groceries', 100.00, user1_id)
    models.insert_expense('2025-10-25', transport_cat['id'], 'Gas', 50.00, user1_id)
    
    # Diana's expenses
    models.insert_expense('2025-10-25', food_cat['id'], 'Restaurant', 75.00, user2_id)
    models.insert_expense('2025-10-25', transport_cat['id'], 'Uber', 25.00, user2_id)
    
    # When: The user requests an expense summary
    summary = expense_operations.calculate_summary()
    
    # Then: The summary should show correct totals
    assert summary['total'] == 250.00, "Total should be sum of all expenses"
    assert summary['count'] == 4, "Should have 4 expenses"
    
    # And: The summary should break down by user
    assert summary['by_user']['Charlie'] == 150.00, "Charlie's total should be 150.00"
    assert summary['by_user']['Diana'] == 100.00, "Diana's total should be 100.00"
    
    # And: The summary should break down by category
    assert summary['by_category'][food_cat['name']] == 175.00, "Food total should be 175.00"
    assert summary['by_category'][transport_cat['name']] == 75.00, "Transport total should be 75.00"
    
    # And: The summary should group expenses by user
    assert 'user_expenses' in summary
    assert len(summary['user_expenses']['Charlie']) == 2, "Charlie should have 2 expenses"
    assert len(summary['user_expenses']['Diana']) == 2, "Diana should have 2 expenses"


def test_user_can_manage_categories(clean_db):
    """
    User Story: As a user, I want to manage categories
    so that I can organize my expenses
    """
    # Given: Default categories exist
    initial_categories = models.get_all_categories()
    initial_count = len(initial_categories)
    assert initial_count >= 7, "Should have default categories"
    
    # When: The user adds a new category
    new_category_id = models.create_category("Custom Category")
    
    # Then: The category should be created
    assert new_category_id > 0, "Category should be created with valid ID"
    
    # And: The category should appear in the list
    all_categories = models.get_all_categories()
    assert len(all_categories) == initial_count + 1, "Should have one more category"
    
    category_names = [cat['name'] for cat in all_categories]
    assert "Custom Category" in category_names, "New category should be in list"
    
    # And: The user can retrieve the category by name
    retrieved_category = models.get_category_by_name("Custom Category")
    assert retrieved_category is not None, "Should find the category"
    assert retrieved_category['name'] == "Custom Category"


def test_user_can_manage_users(clean_db):
    """
    User Story: As a user, I want to manage users
    so that multiple people can track their expenses
    """
    # Given: No users exist initially
    initial_users = models.get_all_users()
    assert len(initial_users) == 0, "Should start with no users"
    
    # When: Multiple users are added
    user1_id = models.create_user("Emma")
    user2_id = models.create_user("Frank")
    user3_id = models.create_user("Grace")
    
    # Then: All users should be created
    assert user1_id > 0 and user2_id > 0 and user3_id > 0, "All users should have valid IDs"
    
    # And: All users should appear in the list
    all_users = models.get_all_users()
    assert len(all_users) == 3, "Should have 3 users"
    
    user_names = [user['name'] for user in all_users]
    assert "Emma" in user_names, "Emma should be in list"
    assert "Frank" in user_names, "Frank should be in list"
    assert "Grace" in user_names, "Grace should be in list"
    
    # And: Users can be retrieved by name
    emma = models.get_user_by_name("Emma")
    assert emma is not None, "Should find Emma"
    assert emma['name'] == "Emma"


def test_expense_validation_prevents_invalid_data(clean_db):
    """
    User Story: As a system, I want to validate expense data
    so that only valid expenses are recorded
    """
    # Given: A user attempts to record expenses with invalid data
    user_id = models.create_user("Validator")
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    # When/Then: Invalid date format should be rejected
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='invalid-date',
            category=categories[0]['name'],
            title='Test',
            amount=50.00,
            user_name='Validator'
        )
    
    # When/Then: Negative amount should be rejected
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=categories[0]['name'],
            title='Test',
            amount=-10.00,
            user_name='Validator'
        )
    
    # When/Then: Zero amount should be rejected
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=categories[0]['name'],
            title='Test',
            amount=0.00,
            user_name='Validator'
        )
    
    # When/Then: Empty title should be rejected
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=categories[0]['name'],
            title='',
            amount=50.00,
            user_name='Validator'
        )
    
    # When/Then: Empty user name should be rejected
    with pytest.raises(ValueError):
        expense_operations.record_expense(
            date='2025-10-25',
            category=categories[0]['name'],
            title='Test',
            amount=50.00,
            user_name=''
        )
    
    # And: No invalid expenses should be recorded
    expenses = expense_operations.view_all_expenses()
    assert len(expenses) == 0, "No invalid expenses should be recorded"


def test_user_can_filter_by_amount_range(clean_db):
    """
    User Story: As a user, I want to filter expenses by amount
    so that I can find large or small expenses
    """
    # Given: Multiple expenses with different amounts exist
    user_id = models.create_user("Henry")
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    models.insert_expense('2025-10-25', category_id, 'Small 1', 5.00, user_id)
    models.insert_expense('2025-10-25', category_id, 'Medium 1', 50.00, user_id)
    models.insert_expense('2025-10-25', category_id, 'Large 1', 500.00, user_id)
    models.insert_expense('2025-10-25', category_id, 'Medium 2', 75.00, user_id)
    models.insert_expense('2025-10-25', category_id, 'Small 2', 10.00, user_id)
    
    # When: The user filters by amount range
    medium_expenses = expense_operations.view_expenses_by_amount(
        min_amount=40.00,
        max_amount=100.00
    )
    
    # Then: Only expenses within the amount range should be returned
    assert len(medium_expenses) == 2, "Should return 2 medium expenses"
    
    for expense in medium_expenses:
        assert 40.00 <= expense['amount'] <= 100.00, \
            "All expenses should be within amount range"


def test_user_can_filter_by_category(clean_db):
    """
    User Story: As a user, I want to filter expenses by category
    so that I can see spending in specific areas
    """
    # Given: Expenses exist in multiple categories
    user_id = models.create_user("Iris")
    categories = models.get_all_categories()
    
    cat1 = categories[0]
    cat2 = categories[1]
    cat3 = categories[2]
    
    models.insert_expense('2025-10-25', cat1['id'], 'Cat1 Exp1', 100.00, user_id)
    models.insert_expense('2025-10-25', cat2['id'], 'Cat2 Exp1', 50.00, user_id)
    models.insert_expense('2025-10-25', cat1['id'], 'Cat1 Exp2', 75.00, user_id)
    models.insert_expense('2025-10-25', cat3['id'], 'Cat3 Exp1', 25.00, user_id)
    models.insert_expense('2025-10-25', cat2['id'], 'Cat2 Exp2', 60.00, user_id)
    
    # When: The user filters by a specific category
    cat1_expenses = expense_operations.view_expenses_by_category([cat1['name']])
    
    # Then: Only expenses from that category should be returned
    assert len(cat1_expenses) == 2, "Should return 2 expenses from category 1"
    
    for expense in cat1_expenses:
        assert expense['category_name'] == cat1['name'], \
            "All expenses should be from the specified category"
    
    # When: The user filters by multiple categories
    multi_cat_expenses = expense_operations.view_expenses_by_category([cat1['name'], cat2['name']])
    
    # Then: Expenses from all specified categories should be returned
    assert len(multi_cat_expenses) == 4, "Should return 4 expenses from both categories"


def test_user_can_track_expenses_over_time(clean_db):
    """
    User Story: As a user, I want to track expenses over time
    so that I can see spending trends
    """
    # Given: Expenses are recorded over multiple months
    user_id = models.create_user("Jack")
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    # October expenses
    models.insert_expense('2025-10-05', category_id, 'Oct Week 1', 100.00, user_id)
    models.insert_expense('2025-10-12', category_id, 'Oct Week 2', 150.00, user_id)
    models.insert_expense('2025-10-19', category_id, 'Oct Week 3', 200.00, user_id)
    models.insert_expense('2025-10-26', category_id, 'Oct Week 4', 175.00, user_id)
    
    # November expenses
    models.insert_expense('2025-11-02', category_id, 'Nov Week 1', 125.00, user_id)
    models.insert_expense('2025-11-09', category_id, 'Nov Week 2', 180.00, user_id)
    
    # When: The user views expenses for October
    october_expenses = expense_operations.view_expenses_by_date(
        min_date='2025-10-01',
        max_date='2025-10-31'
    )
    
    # Then: Only October expenses should be returned
    assert len(october_expenses) == 4, "Should have 4 October expenses"
    
    october_summary = expense_operations.calculate_summary(october_expenses)
    assert october_summary['total'] == 625.00, "October total should be 625.00"
    
    # When: The user views expenses for November
    november_expenses = expense_operations.view_expenses_by_date(
        min_date='2025-11-01',
        max_date='2025-11-30'
    )
    
    # Then: Only November expenses should be returned
    assert len(november_expenses) == 2, "Should have 2 November expenses"
    
    november_summary = expense_operations.calculate_summary(november_expenses)
    assert november_summary['total'] == 305.00, "November total should be 305.00"


def test_multiple_users_independent_tracking(clean_db):
    """
    User Story: As a user, I want my expenses tracked separately from other users
    so that each person's spending is independent
    """
    # Given: Multiple users record expenses
    user1_id = models.create_user("Kate")
    user2_id = models.create_user("Leo")
    
    categories = models.get_all_categories()
    category_id = categories[0]['id']
    
    # Kate's expenses
    models.insert_expense('2025-10-25', category_id, 'Kate Exp 1', 100.00, user1_id)
    models.insert_expense('2025-10-25', category_id, 'Kate Exp 2', 50.00, user1_id)
    
    # Leo's expenses
    models.insert_expense('2025-10-25', category_id, 'Leo Exp 1', 200.00, user2_id)
    models.insert_expense('2025-10-25', category_id, 'Leo Exp 2', 75.00, user2_id)
    models.insert_expense('2025-10-25', category_id, 'Leo Exp 3', 25.00, user2_id)
    
    # When: Each user views their expenses
    kate_expenses = expense_operations.view_expenses_by_user(user1_id)
    leo_expenses = expense_operations.view_expenses_by_user(user2_id)
    
    # Then: Each user should only see their own expenses
    assert len(kate_expenses) == 2, "Kate should have 2 expenses"
    assert len(leo_expenses) == 3, "Leo should have 3 expenses"
    
    # And: Expenses should belong to the correct user
    for expense in kate_expenses:
        assert expense['user_name'] == 'Kate', "All expenses should belong to Kate"
    
    for expense in leo_expenses:
        assert expense['user_name'] == 'Leo', "All expenses should belong to Leo"
    
    # And: Totals should be calculated correctly per user
    summary = expense_operations.calculate_summary()
    assert summary['by_user']['Kate'] == 150.00, "Kate's total should be 150.00"
    assert summary['by_user']['Leo'] == 300.00, "Leo's total should be 300.00"

