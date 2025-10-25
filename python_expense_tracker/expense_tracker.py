"""
Main application entry point for Expense Tracking System
Interactive CLI menu system for expense management
"""

import sys
from typing import List, Dict
import database
import models
import expense_operations
import utils


def display_main_menu() -> None:
    """Display the main menu options"""
    options = [
        "Record New Expense",
        "View All Expenses", 
        "Filter Expenses by Date Range",
        "Filter Expenses by Amount Range",
        "Filter Expenses by Category",
        "Filter Expenses by User",
        "View Expense Summary",
        "Manage Users",
        "Manage Categories"
    ]
    utils.display_menu(options, "Expense Tracking System")


def record_new_expense() -> None:
    """Handle recording a new expense"""
    print("\n=== Record New Expense ===")
    
    try:
        # Get user input with validation
        date = utils.get_user_input("Enter date (YYYY-MM-DD): ")
        date = utils.validate_date(date)
        
        title = utils.get_user_input("Enter expense title: ")
        title = utils.validate_non_empty(title, "Title")
        
        amount_str = utils.get_user_input("Enter amount: ")
        amount = utils.validate_amount(amount_str)
        
        user_name = utils.get_user_input("Enter your name: ")
        user_name = utils.validate_non_empty(user_name, "Name")
        
        # Show available categories
        categories = models.get_all_categories()
        print(f"\nAvailable categories:")
        for i, cat in enumerate(categories, 1):
            print(f"{i}. {cat['name']}")
        
        # Get category selection
        while True:
            try:
                choice = input(f"\nEnter category number (1-{len(categories)}) or enter new category name: ").strip()
                
                # Check if it's a number (existing category)
                if choice.isdigit():
                    choice_num = int(choice)
                    if 1 <= choice_num <= len(categories):
                        category = categories[choice_num - 1]['name']
                        break
                    else:
                        print(f"Please enter a number between 1 and {len(categories)}")
                else:
                    # Treat as new category name
                    category = utils.validate_non_empty(choice, "Category")
                    break
            except ValueError:
                print("Please enter a valid number or category name")
            except KeyboardInterrupt:
                print("\nOperation cancelled.")
                return
        
        # Record the expense
        expense_id = expense_operations.record_expense(date, category, title, amount, user_name)
        print(f"\nExpense recorded successfully!")
        print(f"Expense ID: {expense_id}")
        print(f"Date: {date}")
        print(f"Title: {title}")
        print(f"Amount: {utils.format_currency(amount)}")
        print(f"Category: {category}")
        print(f"User: {user_name}")
        
    except ValueError as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nOperation cancelled.")
    except Exception as e:
        print(f"Unexpected error: {e}")


def view_all_expenses() -> None:
    """Display all expenses"""
    print("\n=== All Expenses ===")
    
    try:
        expenses = expense_operations.view_all_expenses()
        if expenses:
            print(utils.format_expense_output(expenses))
        else:
            print("No expenses found.")
    except Exception as e:
        print(f"Error retrieving expenses: {e}")


def filter_by_date_range() -> None:
    """Filter and display expenses by date range"""
    try:
        expenses = expense_operations.get_expenses_by_date_range()
        if expenses:
            print(f"\nFound {len(expenses)} expense(s):")
            print(utils.format_expense_output(expenses))
        else:
            print("No expenses found for the specified date range.")
    except Exception as e:
        print(f"Error filtering expenses by date: {e}")


def filter_by_amount_range() -> None:
    """Filter and display expenses by amount range"""
    try:
        expenses = expense_operations.get_expenses_by_amount_range()
        if expenses:
            print(f"\nFound {len(expenses)} expense(s):")
            print(utils.format_expense_output(expenses))
        else:
            print("No expenses found for the specified amount range.")
    except Exception as e:
        print(f"Error filtering expenses by amount: {e}")


def filter_by_category() -> None:
    """Filter and display expenses by category"""
    try:
        expenses = expense_operations.get_expenses_by_category_interactive()
        if expenses:
            print(f"\nFound {len(expenses)} expense(s):")
            print(utils.format_expense_output(expenses))
        else:
            print("No expenses found for the selected categories.")
    except Exception as e:
        print(f"Error filtering expenses by category: {e}")


def filter_by_user() -> None:
    """Filter and display expenses by user"""
    try:
        expenses = expense_operations.get_expenses_by_user_interactive()
        if expenses:
            print(f"\nFound {len(expenses)} expense(s):")
            print(utils.format_expense_output(expenses))
        else:
            print("No expenses found for the selected user.")
    except Exception as e:
        print(f"Error filtering expenses by user: {e}")


def view_expense_summary() -> None:
    """Display expense summary"""
    print("\n=== Expense Summary ===")
    
    try:
        summary = expense_operations.calculate_summary()
        print(utils.format_summary_output(summary))
    except Exception as e:
        print(f"Error calculating summary: {e}")


def manage_users() -> None:
    """Manage users (view, add)"""
    print("\n=== Manage Users ===")
    
    while True:
        options = ["View All Users", "Add New User", "Back to Main Menu"]
        utils.display_menu(options, "User Management")
        choice = utils.get_menu_choice(3)
        
        if choice == 0:
            return
        elif choice == 1:
            # View all users
            try:
                users = models.get_all_users()
                if users:
                    print(f"\nAll Users ({len(users)}):")
                    for user in users:
                        print(f"ID: {user['id']}, Name: {user['name']}")
                else:
                    print("No users found.")
            except Exception as e:
                print(f"Error retrieving users: {e}")
        
        elif choice == 2:
            # Add new user
            try:
                name = utils.get_user_input("Enter new user name: ")
                name = utils.validate_non_empty(name, "Name")
                
                user_id = models.create_user(name)
                print(f"User '{name}' created successfully with ID: {user_id}")
            except ValueError as e:
                print(f"Error: {e}")
            except Exception as e:
                print(f"Unexpected error: {e}")
        
        elif choice == 3:
            break


def manage_categories() -> None:
    """Manage categories (view, add)"""
    print("\n=== Manage Categories ===")
    
    while True:
        options = ["View All Categories", "Add New Category", "Back to Main Menu"]
        utils.display_menu(options, "Category Management")
        choice = utils.get_menu_choice(3)
        
        if choice == 0:
            return
        elif choice == 1:
            # View all categories
            try:
                categories = models.get_all_categories()
                if categories:
                    print(f"\nAll Categories ({len(categories)}):")
                    for category in categories:
                        print(f"ID: {category['id']}, Name: {category['name']}")
                else:
                    print("No categories found.")
            except Exception as e:
                print(f"Error retrieving categories: {e}")
        
        elif choice == 2:
            # Add new category
            try:
                name = utils.get_user_input("Enter new category name: ")
                name = utils.validate_non_empty(name, "Category name")
                
                category_id = models.create_category(name)
                print(f"Category '{name}' created successfully with ID: {category_id}")
            except ValueError as e:
                print(f"Error: {e}")
            except Exception as e:
                print(f"Unexpected error: {e}")
        
        elif choice == 3:
            break


def main() -> None:
    """Main application loop"""
    print("Welcome to the Expense Tracking System!")
    print("Initializing database...")
    
    try:
        # Initialize database
        database.init_database()
        print("Database initialized successfully!")
    except Exception as e:
        print(f"Failed to initialize database: {e}")
        print("Exiting application.")
        sys.exit(1)
    
    # Main application loop
    while True:
        try:
            display_main_menu()
            choice = utils.get_menu_choice(9)
            
            if choice == 0:
                print("\nThank you for using the Expense Tracking System!")
                break
            elif choice == 1:
                record_new_expense()
            elif choice == 2:
                view_all_expenses()
            elif choice == 3:
                filter_by_date_range()
            elif choice == 4:
                filter_by_amount_range()
            elif choice == 5:
                filter_by_category()
            elif choice == 6:
                filter_by_user()
            elif choice == 7:
                view_expense_summary()
            elif choice == 8:
                manage_users()
            elif choice == 9:
                manage_categories()
            
            # Pause before showing menu again
            if choice != 0:
                input("\nPress Enter to continue...")
                
        except KeyboardInterrupt:
            print("\n\nApplication interrupted. Goodbye!")
            break
        except Exception as e:
            print(f"\nUnexpected error: {e}")
            print("Please try again or contact support if the problem persists.")


if __name__ == "__main__":
    main()
