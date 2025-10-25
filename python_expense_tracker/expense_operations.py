"""
Core business logic for Expense Tracking System
Implements all required core functions for expense management
"""

from typing import List, Dict, Optional, Any
from datetime import datetime
import models
import utils


def record_expense(date: str, category: str, title: str, amount: float, user_name: str) -> int:
    """
    Record a new expense with validation and user/category resolution
    
    Args:
        date: Expense date in YYYY-MM-DD format
        category: Category name
        title: Expense title/description
        amount: Expense amount
        user_name: Name of the user making the expense
        
    Returns:
        int: Expense ID of the created expense
        
    Raises:
        ValueError: If inputs are invalid
        sqlite3.IntegrityError: If user/category doesn't exist and can't be created
    """
    # Validate inputs
    date = utils.validate_date(date)
    amount = utils.validate_amount(str(amount))
    title = utils.validate_non_empty(title, "Title")
    user_name = utils.validate_non_empty(user_name, "User name")
    category = utils.validate_non_empty(category, "Category")
    
    # Get or create user
    user = models.get_user_by_name(user_name)
    if not user:
        try:
            user_id = models.create_user(user_name)
            print(f"Created new user: {user_name}")
        except Exception as e:
            raise ValueError(f"Failed to create user '{user_name}': {e}")
    else:
        user_id = user['id']
    
    # Get or create category
    category_obj = models.get_category_by_name(category)
    if not category_obj:
        try:
            category_id = models.create_category(category)
            print(f"Created new category: {category}")
        except Exception as e:
            raise ValueError(f"Failed to create category '{category}': {e}")
    else:
        category_id = category_obj['id']
    
    # Insert expense
    try:
        expense_id = models.insert_expense(date, category_id, title, amount, user_id)
        print(f"Expense recorded successfully with ID: {expense_id}")
        return expense_id
    except Exception as e:
        raise ValueError(f"Failed to record expense: {e}")


def view_expenses_by_date(min_date: Optional[str] = None, max_date: Optional[str] = None) -> List[Dict]:
    """
    View expenses filtered by date range
    
    Args:
        min_date: Minimum date (YYYY-MM-DD format)
        max_date: Maximum date (YYYY-MM-DD format)
        
    Returns:
        List[Dict]: List of expense dictionaries
    """
    try:
        # Validate dates if provided
        if min_date:
            min_date = utils.validate_date(min_date)
        if max_date:
            max_date = utils.validate_date(max_date)
        
        # Validate date range
        if min_date and max_date and min_date > max_date:
            raise ValueError("Start date cannot be after end date")
        
        expenses = models.fetch_expenses_by_filters(min_date=min_date, max_date=max_date)
        return expenses
    except Exception as e:
        print(f"Error filtering expenses by date: {e}")
        return []


def view_expenses_by_amount(min_amount: Optional[float] = None, max_amount: Optional[float] = None) -> List[Dict]:
    """
    View expenses filtered by amount range
    
    Args:
        min_amount: Minimum amount
        max_amount: Maximum amount
        
    Returns:
        List[Dict]: List of expense dictionaries
    """
    try:
        # Validate amounts if provided
        if min_amount is not None and min_amount < 0:
            raise ValueError("Minimum amount cannot be negative")
        if max_amount is not None and max_amount < 0:
            raise ValueError("Maximum amount cannot be negative")
        if min_amount is not None and max_amount is not None and min_amount > max_amount:
            raise ValueError("Minimum amount cannot be greater than maximum amount")
        
        expenses = models.fetch_expenses_by_filters(min_amount=min_amount, max_amount=max_amount)
        return expenses
    except Exception as e:
        print(f"Error filtering expenses by amount: {e}")
        return []


def view_expenses_by_category(categories: List[str]) -> List[Dict]:
    """
    View expenses filtered by category list
    
    Args:
        categories: List of category names to filter by
        
    Returns:
        List[Dict]: List of expense dictionaries
    """
    try:
        if not categories:
            return []
        
        # Get category IDs for the provided category names
        all_categories = models.get_all_categories()
        category_lookup = {cat['name'].lower(): cat['id'] for cat in all_categories}
        
        category_ids = []
        invalid_categories = []
        
        for category in categories:
            category_lower = category.lower()
            if category_lower in category_lookup:
                category_ids.append(category_lookup[category_lower])
            else:
                invalid_categories.append(category)
        
        if invalid_categories:
            print(f"Warning: Categories not found: {', '.join(invalid_categories)}")
        
        if not category_ids:
            print("No valid categories found.")
            return []
        
        expenses = models.fetch_expenses_by_filters(category_ids=category_ids)
        return expenses
    except Exception as e:
        print(f"Error filtering expenses by category: {e}")
        return []


def view_expenses_by_user(user_id: int) -> List[Dict]:
    """
    View expenses filtered by user ID
    
    Args:
        user_id: ID of the user to filter by
        
    Returns:
        List[Dict]: List of expense dictionaries
    """
    try:
        if user_id <= 0:
            raise ValueError("User ID must be positive")
        
        expenses = models.fetch_expenses_by_filters(user_id=user_id)
        return expenses
    except Exception as e:
        print(f"Error filtering expenses by user: {e}")
        return []


def view_all_expenses() -> List[Dict]:
    """
    View all expenses in the system
    
    Returns:
        List[Dict]: List of all expense dictionaries
    """
    try:
        expenses = models.fetch_expenses_by_filters()
        return expenses
    except Exception as e:
        print(f"Error retrieving all expenses: {e}")
        return []


def calculate_summary(expenses: Optional[List[Dict]] = None) -> Dict[str, Any]:
    """
    Calculate expense summary with totals and breakdowns
    
    Args:
        expenses: List of expenses to summarize (if None, uses all expenses)
        
    Returns:
        Dict: Summary dictionary with totals and breakdowns
    """
    try:
        if expenses is None:
            expenses = view_all_expenses()
        
        if not expenses:
            return {
                'total': 0.0,
                'count': 0,
                'by_category': {},
                'by_user': {}
            }
        
        # Calculate totals using list comprehensions (Python feature)
        total = sum(expense['amount'] for expense in expenses)
        count = len(expenses)
        
        # Calculate by category using dictionary comprehension (Python feature)
        by_category = {}
        for expense in expenses:
            category = expense['category_name']
            by_category[category] = by_category.get(category, 0) + expense['amount']
        
        # Calculate by user using dictionary comprehension (Python feature)
        by_user = {}
        for expense in expenses:
            user = expense['user_name']
            by_user[user] = by_user.get(user, 0) + expense['amount']
        
        return {
            'total': total,
            'count': count,
            'by_category': by_category,
            'by_user': by_user
        }
    except Exception as e:
        print(f"Error calculating summary: {e}")
        return {
            'total': 0.0,
            'count': 0,
            'by_category': {},
            'by_user': {}
        }


def get_expenses_by_date_range() -> List[Dict]:
    """
    Interactive function to get expenses by date range with user input
    
    Returns:
        List[Dict]: Filtered expenses
    """
    print("\n=== Filter Expenses by Date Range ===")
    min_date, max_date = utils.get_date_range_input()
    
    if min_date is None and max_date is None:
        print("No date range specified. Showing all expenses.")
        return view_all_expenses()
    
    return view_expenses_by_date(min_date, max_date)


def get_expenses_by_amount_range() -> List[Dict]:
    """
    Interactive function to get expenses by amount range with user input
    
    Returns:
        List[Dict]: Filtered expenses
    """
    print("\n=== Filter Expenses by Amount Range ===")
    min_amount, max_amount = utils.get_amount_range_input()
    
    if min_amount is None and max_amount is None:
        print("No amount range specified. Showing all expenses.")
        return view_all_expenses()
    
    return view_expenses_by_amount(min_amount, max_amount)


def get_expenses_by_category_interactive() -> List[Dict]:
    """
    Interactive function to get expenses by category with user input
    
    Returns:
        List[Dict]: Filtered expenses
    """
    print("\n=== Filter Expenses by Category ===")
    
    # Show available categories
    categories = models.get_all_categories()
    if not categories:
        print("No categories available.")
        return []
    
    print("Available categories:")
    for i, cat in enumerate(categories, 1):
        print(f"{i}. {cat['name']}")
    
    # Get user selection
    selected_categories = []
    while True:
        try:
            choice = input("\nEnter category number (or 'done' to finish): ").strip()
            if choice.lower() == 'done':
                break
            
            choice_num = int(choice)
            if 1 <= choice_num <= len(categories):
                selected_cat = categories[choice_num - 1]['name']
                if selected_cat not in selected_categories:
                    selected_categories.append(selected_cat)
                    print(f"Added category: {selected_cat}")
                else:
                    print(f"Category {selected_cat} already selected")
            else:
                print(f"Please enter a number between 1 and {len(categories)}")
        except ValueError:
            print("Please enter a valid number or 'done'")
        except KeyboardInterrupt:
            print("\nOperation cancelled.")
            return []
    
    if not selected_categories:
        print("No categories selected. Showing all expenses.")
        return view_all_expenses()
    
    return view_expenses_by_category(selected_categories)


def get_expenses_by_user_interactive() -> List[Dict]:
    """
    Interactive function to get expenses by user with user input
    
    Returns:
        List[Dict]: Filtered expenses
    """
    print("\n=== Filter Expenses by User ===")
    
    # Show available users
    users = models.get_all_users()
    if not users:
        print("No users available.")
        return []
    
    print("Available users:")
    for i, user in enumerate(users, 1):
        print(f"{i}. {user['name']}")
    
    # Get user selection
    try:
        choice = input(f"\nEnter user number (1-{len(users)}): ").strip()
        choice_num = int(choice)
        if 1 <= choice_num <= len(users):
            selected_user = users[choice_num - 1]
            return view_expenses_by_user(selected_user['id'])
        else:
            print(f"Please enter a number between 1 and {len(users)}")
            return []
    except ValueError:
        print("Please enter a valid number")
        return []
    except KeyboardInterrupt:
        print("\nOperation cancelled.")
        return []
