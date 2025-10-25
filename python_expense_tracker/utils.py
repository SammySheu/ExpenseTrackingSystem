"""
Utility functions for Expense Tracking System
Handles validation, formatting, and helper functions
"""

from datetime import datetime
from typing import List, Dict, Any, Optional
import re


def validate_date(date_string: str) -> str:
    """
    Parse and validate date format (YYYY-MM-DD)
    
    Args:
        date_string: Date string to validate
        
    Returns:
        str: Validated date string in YYYY-MM-DD format
        
    Raises:
        ValueError: If date format is invalid or date is invalid
    """
    if not date_string or not date_string.strip():
        raise ValueError("Date cannot be empty")
    
    date_string = date_string.strip()
    
    # Check format with regex
    if not re.match(r'^\d{4}-\d{2}-\d{2}$', date_string):
        raise ValueError("Date must be in YYYY-MM-DD format")
    
    try:
        # Parse the date to validate it's a real date
        datetime.strptime(date_string, '%Y-%m-%d')
        return date_string
    except ValueError as e:
        raise ValueError(f"Invalid date: {e}")


def validate_amount(amount_str: str) -> float:
    """
    Validate and convert amount string to float
    
    Args:
        amount_str: Amount string to validate
        
    Returns:
        float: Validated amount as float
        
    Raises:
        ValueError: If amount is invalid or not positive
    """
    if not amount_str or not amount_str.strip():
        raise ValueError("Amount cannot be empty")
    
    try:
        amount = float(amount_str.strip())
        if amount <= 0:
            raise ValueError("Amount must be positive")
        return amount
    except ValueError as e:
        if "could not convert" in str(e):
            raise ValueError("Amount must be a valid number")
        raise


def validate_non_empty(value: str, field_name: str) -> str:
    """
    Check that a required field is not empty
    
    Args:
        value: Value to validate
        field_name: Name of the field for error messages
        
    Returns:
        str: Validated value (stripped)
        
    Raises:
        ValueError: If value is empty
    """
    if not value or not value.strip():
        raise ValueError(f"{field_name} cannot be empty")
    return value.strip()


def format_currency(amount: float) -> str:
    """
    Format amount as currency with 2 decimal places
    
    Args:
        amount: Amount to format
        
    Returns:
        str: Formatted currency string
    """
    return f"${amount:.2f}"


def format_expense_output(expenses: List[Dict]) -> str:
    """
    Pretty-print expense records in table format
    
    Args:
        expenses: List of expense dictionaries
        
    Returns:
        str: Formatted table string
    """
    if not expenses:
        return "No expenses found."
    
    # Calculate column widths
    max_id_width = max(len(str(exp['id'])) for exp in expenses)
    max_date_width = max(len(exp['date']) for exp in expenses)
    max_title_width = max(len(exp['title']) for exp in expenses)
    max_amount_width = max(len(format_currency(exp['amount'])) for exp in expenses)
    max_category_width = max(len(exp['category_name']) for exp in expenses)
    max_user_width = max(len(exp['user_name']) for exp in expenses)
    
    # Set minimum widths
    id_width = max(max_id_width, 3)
    date_width = max(max_date_width, 10)
    title_width = max(max_title_width, 5)
    amount_width = max(max_amount_width, 7)
    category_width = max(max_category_width, 8)
    user_width = max(max_user_width, 4)
    
    # Create header
    header = f"{'ID':<{id_width}} | {'Date':<{date_width}} | {'Title':<{title_width}} | {'Amount':<{amount_width}} | {'Category':<{category_width}} | {'User':<{user_width}}"
    separator = "-" * len(header)
    
    # Create rows
    rows = []
    for expense in expenses:
        row = f"{expense['id']:<{id_width}} | {expense['date']:<{date_width}} | {expense['title']:<{title_width}} | {format_currency(expense['amount']):<{amount_width}} | {expense['category_name']:<{category_width}} | {expense['user_name']:<{user_width}}"
        rows.append(row)
    
    return "\n".join([header, separator] + rows)


def format_summary_output(summary: Dict[str, Any]) -> str:
    """
    Format expense summary in a readable format
    Shows: 1) Each User's Expenses in Separate Tables, 2) Total Expenses, 3) Percentage by Category
    
    Args:
        summary: Summary dictionary with totals, category breakdowns, and user expense details
        
    Returns:
        str: Formatted summary string
    """
    output = []
    output.append("=== EXPENSE SUMMARY ===\n")
    
    # 1. Show each user's expenses in separate tables
    if summary.get('user_expenses'):
        output.append("EXPENSES BY USER:")
        output.append("=" * 80)
        
        for user, expenses in summary['user_expenses'].items():
            output.append(f"\n{user}'s Expenses:")
            output.append("-" * 80)
            
            # Create table for this user's expenses
            if expenses:
                # Calculate column widths for this user's expenses
                max_date_width = max(len(exp['date']) for exp in expenses)
                max_title_width = max(len(exp['title']) for exp in expenses)
                max_amount_width = max(len(format_currency(exp['amount'])) for exp in expenses)
                max_category_width = max(len(exp['category_name']) for exp in expenses)
                
                # Set minimum widths
                date_width = max(max_date_width, 10)
                title_width = max(max_title_width, 15)
                amount_width = max(max_amount_width, 8)
                category_width = max(max_category_width, 10)
                
                # Create header for user's table
                header = f"{'Date':<{date_width}} | {'Title':<{title_width}} | {'Amount':<{amount_width}} | {'Category':<{category_width}}"
                output.append(header)
                output.append("-" * len(header))
                
                # Add expense rows
                for expense in expenses:
                    row = f"{expense['date']:<{date_width}} | {expense['title']:<{title_width}} | {format_currency(expense['amount']):<{amount_width}} | {expense['category_name']:<{category_width}}"
                    output.append(row)
                
                # Add user's total
                user_total = summary['by_user'][user]
                output.append("-" * len(header))
                output.append(f"{'TOTAL':<{date_width}} | {'':<{title_width}} | {format_currency(user_total):<{amount_width}} | {len(expenses)} expense(s)")
            
            output.append("")
        
        output.append("=" * 80)
        output.append("")
    
    # 2. Show total expenses
    output.append("OVERALL SUMMARY:")
    output.append(f"Total Expenses: {format_currency(summary['total'])}")
    output.append(f"Number of Expenses: {summary['count']}")
    output.append("")
    
    # 3. Show percentage breakdown by category
    if summary['by_category'] and summary['total'] > 0:
        output.append("CATEGORY BREAKDOWN (with Percentages):")
        # Sort categories by amount (descending) for better readability
        sorted_categories = sorted(summary['by_category'].items(), key=lambda x: x[1], reverse=True)
        
        for category, amount in sorted_categories:
            percentage = (amount / summary['total']) * 100
            output.append(f"  {category}: {format_currency(amount)} ({percentage:.1f}%)")
    
    return "\n".join(output)


def get_date_range_input() -> tuple[Optional[str], Optional[str]]:
    """
    Get date range input from user with validation
    
    Returns:
        tuple: (min_date, max_date) or (None, None) if cancelled
    """
    print("\nEnter date range (YYYY-MM-DD format, leave empty to skip):")
    
    min_date = None
    max_date = None
    
    try:
        min_input = input("Start date (min): ").strip()
        if min_input:
            min_date = validate_date(min_input)
        
        max_input = input("End date (max): ").strip()
        if max_input:
            max_date = validate_date(max_input)
        
        # Validate date range
        if min_date and max_date and min_date > max_date:
            raise ValueError("Start date cannot be after end date")
        
        return min_date, max_date
    except ValueError as e:
        print(f"Error: {e}")
        return None, None


def get_amount_range_input() -> tuple[Optional[float], Optional[float]]:
    """
    Get amount range input from user with validation
    
    Returns:
        tuple: (min_amount, max_amount) or (None, None) if cancelled
    """
    print("\nEnter amount range (leave empty to skip):")
    
    min_amount = None
    max_amount = None
    
    try:
        min_input = input("Minimum amount: ").strip()
        if min_input:
            min_amount = validate_amount(min_input)
        
        max_input = input("Maximum amount: ").strip()
        if max_input:
            max_amount = validate_amount(max_input)
        
        # Validate amount range
        if min_amount is not None and max_amount is not None and min_amount > max_amount:
            raise ValueError("Minimum amount cannot be greater than maximum amount")
        
        return min_amount, max_amount
    except ValueError as e:
        print(f"Error: {e}")
        return None, None


def get_user_input(prompt: str, allow_empty: bool = False) -> str:
    """
    Get user input with optional validation
    
    Args:
        prompt: Prompt to display to user
        allow_empty: Whether empty input is allowed
        
    Returns:
        str: User input (stripped)
    """
    while True:
        try:
            value = input(prompt).strip()
            if not value and not allow_empty:
                print("This field cannot be empty. Please try again.")
                continue
            return value
        except KeyboardInterrupt:
            print("\nOperation cancelled.")
            return ""


def display_menu(options: List[str], title: str = "Menu") -> None:
    """
    Display a numbered menu to the user
    
    Args:
        options: List of menu options
        title: Menu title
    """
    print(f"\n=== {title} ===")
    for i, option in enumerate(options, 1):
        print(f"{i}. {option}")
    print("0. Exit" if "Exit" not in options else "")


def get_menu_choice(max_choice: int) -> int:
    """
    Get a valid menu choice from user
    
    Args:
        max_choice: Maximum valid choice number
        
    Returns:
        int: User's choice (0 to max_choice)
    """
    while True:
        try:
            choice = input(f"\nEnter your choice (0-{max_choice}): ").strip()
            choice_num = int(choice)
            if 0 <= choice_num <= max_choice:
                return choice_num
            else:
                print(f"Please enter a number between 0 and {max_choice}")
        except ValueError:
            print("Please enter a valid number")
        except KeyboardInterrupt:
            print("\nOperation cancelled.")
            return 0
