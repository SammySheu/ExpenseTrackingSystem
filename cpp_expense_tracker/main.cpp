/**
 * main.cpp
 * Main application entry point
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Stack-based object allocation (vs Python's heap allocation)
 * - RAII for automatic resource cleanup
 * - Compile-time type checking
 * - Manual memory management considerations
 * 
 * PROGRAM STRUCTURE:
 * This demonstrates the difference between compiled (C++) and interpreted (Python) languages
 * C++ requires compilation before execution, Python runs directly
 */

#include <iostream>
#include <vector>
#include <string>
#include "database.h"
#include "models.h"
#include "expense_operations.h"
#include "utils.h"

using namespace std;

void displayMainMenu() {
    vector<string> options = {
        "Record New Expense",
        "View All Expenses",
        "Filter Expenses by Date Range",
        "Filter Expenses by Amount Range",
        "Filter Expenses by Category",
        "Filter Expenses by User",
        "View Expense Summary",
        "Manage Users",
        "Manage Categories"
    };
    Utils::displayMenu(options, "Expense Tracking System");
}

void recordNewExpense(Database& db) {
    cout << "\n=== Record New Expense ===" << endl;
    
    try {
        // Get user input with validation
        string date = Utils::getUserInput("Enter date (YYYY-MM-DD): ");
        string validated_date;
        if (!Utils::parseDate(date, validated_date)) {
            cout << "Error: Invalid date format" << endl;
            return;
        }
        
        string title = Utils::getUserInput("Enter expense title: ");
        if (!Utils::validateNonEmpty(title)) {
            cout << "Error: Title cannot be empty" << endl;
            return;
        }
        
        string amount_str = Utils::getUserInput("Enter amount: ");
        double amount;
        if (!Utils::parseAmount(amount_str, amount)) {
            cout << "Error: Invalid amount" << endl;
            return;
        }
        
        string user_name = Utils::getUserInput("Enter your name: ");
        if (!Utils::validateNonEmpty(user_name)) {
            cout << "Error: Name cannot be empty" << endl;
            return;
        }
        
        // Show available categories
        auto categories = Models::getAllCategories(db);
        cout << "\nAvailable categories:" << endl;
        for (size_t i = 0; i < categories.size(); ++i) {
            cout << (i + 1) << ". " << categories[i].name << endl;
        }
        
        // Get category selection
        string category;
        while (true) {
            string choice = Utils::getUserInput(
                "\nEnter category number (1-" + to_string(categories.size()) + 
                ") or enter new category name: ");
            
            try {
                int choice_num = stoi(choice);
                if (choice_num >= 1 && choice_num <= static_cast<int>(categories.size())) {
                    category = categories[choice_num - 1].name;
                    break;
                }
            } catch (const exception&) {
                // Not a number, treat as new category name
                if (Utils::validateNonEmpty(choice)) {
                    category = choice;
                    break;
                }
            }
            cout << "Please enter a valid category number or name" << endl;
        }
        
        // Record the expense
        int expense_id = ExpenseOperations::recordExpense(db, validated_date, category, 
                                                         title, amount, user_name);
        
        cout << "\nExpense recorded successfully!" << endl;
        cout << "Expense ID: " << expense_id << endl;
        cout << "Date: " << validated_date << endl;
        cout << "Title: " << title << endl;
        cout << "Amount: " << Utils::formatCurrency(amount) << endl;
        cout << "Category: " << category << endl;
        cout << "User: " << user_name << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void viewAllExpenses(Database& db) {
    cout << "\n=== All Expenses ===" << endl;
    
    try {
        auto expenses = ExpenseOperations::viewAllExpenses(db);
        if (!expenses.empty()) {
            cout << Utils::formatExpenseOutput(expenses) << endl;
        } else {
            cout << "No expenses found." << endl;
        }
    } catch (const exception& e) {
        cout << "Error retrieving expenses: " << e.what() << endl;
    }
}

void filterByDateRange(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByDateRange(db);
        if (!expenses.empty()) {
            cout << "\nFound " << expenses.size() << " expense(s):" << endl;
            cout << Utils::formatExpenseOutput(expenses) << endl;
        } else {
            cout << "No expenses found for the specified date range." << endl;
        }
    } catch (const exception& e) {
        cout << "Error filtering expenses by date: " << e.what() << endl;
    }
}

void filterByAmountRange(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByAmountRange(db);
        if (!expenses.empty()) {
            cout << "\nFound " << expenses.size() << " expense(s):" << endl;
            cout << Utils::formatExpenseOutput(expenses) << endl;
        } else {
            cout << "No expenses found for the specified amount range." << endl;
        }
    } catch (const exception& e) {
        cout << "Error filtering expenses by amount: " << e.what() << endl;
    }
}

void filterByCategory(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByCategory(db);
        if (!expenses.empty()) {
            cout << "\nFound " << expenses.size() << " expense(s):" << endl;
            cout << Utils::formatExpenseOutput(expenses) << endl;
        } else {
            cout << "No expenses found for the selected categories." << endl;
        }
    } catch (const exception& e) {
        cout << "Error filtering expenses by category: " << e.what() << endl;
    }
}

void filterByUser(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByUser(db);
        if (!expenses.empty()) {
            cout << "\nFound " << expenses.size() << " expense(s):" << endl;
            cout << Utils::formatExpenseOutput(expenses) << endl;
        } else {
            cout << "No expenses found for the selected user." << endl;
        }
    } catch (const exception& e) {
        cout << "Error filtering expenses by user: " << e.what() << endl;
    }
}

void viewExpenseSummary(Database& db) {
    try {
        auto summary = ExpenseOperations::calculateSummary(db);
        cout << Utils::formatSummaryOutput(summary) << endl;
    } catch (const exception& e) {
        cout << "Error calculating summary: " << e.what() << endl;
    }
}

void manageUsers(Database& db) {
    cout << "\n=== Manage Users ===" << endl;
    
    while (true) {
        vector<string> options = {
            "View All Users",
            "Add New User",
            "Back to Main Menu"
        };
        Utils::displayMenu(options, "User Management");
        int choice = Utils::getMenuChoice(3);
        
        if (choice == 0 || choice == 3) {
            return;
        } else if (choice == 1) {
            // View all users
            try {
                auto users = Models::getAllUsers(db);
                if (!users.empty()) {
                    cout << "\nAll Users (" << users.size() << "):" << endl;
                    for (const auto& user : users) {
                        cout << "ID: " << user.id << ", Name: " << user.name << endl;
                    }
                } else {
                    cout << "No users found." << endl;
                }
            } catch (const exception& e) {
                cout << "Error retrieving users: " << e.what() << endl;
            }
        } else if (choice == 2) {
            // Add new user
            try {
                string name = Utils::getUserInput("Enter new user name: ");
                if (Utils::validateNonEmpty(name)) {
                    int user_id = Models::createUser(db, name);
                    cout << "User '" << name << "' created successfully with ID: " 
                             << user_id << endl;
                } else {
                    cout << "Error: Name cannot be empty" << endl;
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
        
        cout << "\nPress Enter to continue...";
        cin.get();
    }
}

void manageCategories(Database& db) {
    cout << "\n=== Manage Categories ===" << endl;
    
    while (true) {
        vector<string> options = {
            "View All Categories",
            "Add New Category",
            "Back to Main Menu"
        };
        Utils::displayMenu(options, "Category Management");
        int choice = Utils::getMenuChoice(3);
        
        if (choice == 0 || choice == 3) {
            return;
        } else if (choice == 1) {
            // View all categories
            try {
                auto categories = Models::getAllCategories(db);
                if (!categories.empty()) {
                    cout << "\nAll Categories (" << categories.size() << "):" << endl;
                    for (const auto& category : categories) {
                        cout << "ID: " << category.id << ", Name: " 
                                 << category.name << endl;
                    }
                } else {
                    cout << "No categories found." << endl;
                }
            } catch (const exception& e) {
                cout << "Error retrieving categories: " << e.what() << endl;
            }
        } else if (choice == 2) {
            // Add new category
            try {
                string name = Utils::getUserInput("Enter new category name: ");
                if (Utils::validateNonEmpty(name)) {
                    int category_id = Models::createCategory(db, name);
                    cout << "Category '" << name << "' created successfully with ID: " 
                             << category_id << endl;
                } else {
                    cout << "Error: Category name cannot be empty" << endl;
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
        
        cout << "\nPress Enter to continue...";
        cin.get();
    }
}

/**
 * C++ FEATURE: main() function as entry point
 * Returns int (exit code) unlike Python's __main__ pattern
 * 
 * RAII DEMONSTRATION:
 * Database object is allocated on stack and automatically cleaned up
 * when it goes out of scope (no manual delete needed)
 */
int main() {
    cout << "Welcome to the Expense Tracking System!" << endl;
    cout << "Initializing database..." << endl;
    
    // C++ FEATURE: Stack allocation with RAII
    // Database destructor will automatically be called when main() exits
    // CONTRAST WITH PYTHON: Python uses 'with' statement or garbage collection
    Database db("expenses.db");
    
    try {
        if (!db.initialize()) {
            cerr << "Failed to initialize database" << endl;
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Failed to initialize database: " << e.what() << endl;
        return 1;
    }
    
    // Main application loop
    while (true) {
        try {
            displayMainMenu();
            int choice = Utils::getMenuChoice(9);
            
            if (choice == 0) {
                cout << "\nThank you for using the Expense Tracking System!" << endl;
                break;
            } else if (choice == 1) {
                recordNewExpense(db);
            } else if (choice == 2) {
                viewAllExpenses(db);
            } else if (choice == 3) {
                filterByDateRange(db);
            } else if (choice == 4) {
                filterByAmountRange(db);
            } else if (choice == 5) {
                filterByCategory(db);
            } else if (choice == 6) {
                filterByUser(db);
            } else if (choice == 7) {
                viewExpenseSummary(db);
            } else if (choice == 8) {
                manageUsers(db);
            } else if (choice == 9) {
                manageCategories(db);
            }
            
            // Pause before showing menu again
            if (choice != 0) {
                cout << "\nPress Enter to continue...";
                cin.get();
            }
            
        } catch (const exception& e) {
            cerr << "\nUnexpected error: " << e.what() << endl;
            cout << "Please try again or contact support if the problem persists." << endl;
        }
    }
    
    // C++ FEATURE: Explicit return code (0 = success)
    // Database destructor is automatically called here
    return 0;
}

