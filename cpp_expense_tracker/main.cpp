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

void displayMainMenu() {
    std::vector<std::string> options = {
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
    std::cout << "\n=== Record New Expense ===" << std::endl;
    
    try {
        // Get user input with validation
        std::string date = Utils::getUserInput("Enter date (YYYY-MM-DD): ");
        std::string validated_date;
        if (!Utils::parseDate(date, validated_date)) {
            std::cout << "Error: Invalid date format" << std::endl;
            return;
        }
        
        std::string title = Utils::getUserInput("Enter expense title: ");
        if (!Utils::validateNonEmpty(title)) {
            std::cout << "Error: Title cannot be empty" << std::endl;
            return;
        }
        
        std::string amount_str = Utils::getUserInput("Enter amount: ");
        double amount;
        if (!Utils::parseAmount(amount_str, amount)) {
            std::cout << "Error: Invalid amount" << std::endl;
            return;
        }
        
        std::string user_name = Utils::getUserInput("Enter your name: ");
        if (!Utils::validateNonEmpty(user_name)) {
            std::cout << "Error: Name cannot be empty" << std::endl;
            return;
        }
        
        // Show available categories
        auto categories = Models::getAllCategories(db);
        std::cout << "\nAvailable categories:" << std::endl;
        for (size_t i = 0; i < categories.size(); ++i) {
            std::cout << (i + 1) << ". " << categories[i].name << std::endl;
        }
        
        // Get category selection
        std::string category;
        while (true) {
            std::string choice = Utils::getUserInput(
                "\nEnter category number (1-" + std::to_string(categories.size()) + 
                ") or enter new category name: ");
            
            try {
                int choice_num = std::stoi(choice);
                if (choice_num >= 1 && choice_num <= static_cast<int>(categories.size())) {
                    category = categories[choice_num - 1].name;
                    break;
                }
            } catch (const std::exception&) {
                // Not a number, treat as new category name
                if (Utils::validateNonEmpty(choice)) {
                    category = choice;
                    break;
                }
            }
            std::cout << "Please enter a valid category number or name" << std::endl;
        }
        
        // Record the expense
        int expense_id = ExpenseOperations::recordExpense(db, validated_date, category, 
                                                         title, amount, user_name);
        
        std::cout << "\nExpense recorded successfully!" << std::endl;
        std::cout << "Expense ID: " << expense_id << std::endl;
        std::cout << "Date: " << validated_date << std::endl;
        std::cout << "Title: " << title << std::endl;
        std::cout << "Amount: " << Utils::formatCurrency(amount) << std::endl;
        std::cout << "Category: " << category << std::endl;
        std::cout << "User: " << user_name << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void viewAllExpenses(Database& db) {
    std::cout << "\n=== All Expenses ===" << std::endl;
    
    try {
        auto expenses = ExpenseOperations::viewAllExpenses(db);
        if (!expenses.empty()) {
            std::cout << Utils::formatExpenseOutput(expenses) << std::endl;
        } else {
            std::cout << "No expenses found." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error retrieving expenses: " << e.what() << std::endl;
    }
}

void filterByDateRange(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByDateRange(db);
        if (!expenses.empty()) {
            std::cout << "\nFound " << expenses.size() << " expense(s):" << std::endl;
            std::cout << Utils::formatExpenseOutput(expenses) << std::endl;
        } else {
            std::cout << "No expenses found for the specified date range." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error filtering expenses by date: " << e.what() << std::endl;
    }
}

void filterByAmountRange(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByAmountRange(db);
        if (!expenses.empty()) {
            std::cout << "\nFound " << expenses.size() << " expense(s):" << std::endl;
            std::cout << Utils::formatExpenseOutput(expenses) << std::endl;
        } else {
            std::cout << "No expenses found for the specified amount range." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error filtering expenses by amount: " << e.what() << std::endl;
    }
}

void filterByCategory(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByCategory(db);
        if (!expenses.empty()) {
            std::cout << "\nFound " << expenses.size() << " expense(s):" << std::endl;
            std::cout << Utils::formatExpenseOutput(expenses) << std::endl;
        } else {
            std::cout << "No expenses found for the selected categories." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error filtering expenses by category: " << e.what() << std::endl;
    }
}

void filterByUser(Database& db) {
    try {
        auto expenses = ExpenseOperations::getExpensesByUser(db);
        if (!expenses.empty()) {
            std::cout << "\nFound " << expenses.size() << " expense(s):" << std::endl;
            std::cout << Utils::formatExpenseOutput(expenses) << std::endl;
        } else {
            std::cout << "No expenses found for the selected user." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error filtering expenses by user: " << e.what() << std::endl;
    }
}

void viewExpenseSummary(Database& db) {
    try {
        auto summary = ExpenseOperations::calculateSummary(db);
        std::cout << Utils::formatSummaryOutput(summary) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error calculating summary: " << e.what() << std::endl;
    }
}

void manageUsers(Database& db) {
    std::cout << "\n=== Manage Users ===" << std::endl;
    
    while (true) {
        std::vector<std::string> options = {
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
                    std::cout << "\nAll Users (" << users.size() << "):" << std::endl;
                    for (const auto& user : users) {
                        std::cout << "ID: " << user.id << ", Name: " << user.name << std::endl;
                    }
                } else {
                    std::cout << "No users found." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error retrieving users: " << e.what() << std::endl;
            }
        } else if (choice == 2) {
            // Add new user
            try {
                std::string name = Utils::getUserInput("Enter new user name: ");
                if (Utils::validateNonEmpty(name)) {
                    int user_id = Models::createUser(db, name);
                    std::cout << "User '" << name << "' created successfully with ID: " 
                             << user_id << std::endl;
                } else {
                    std::cout << "Error: Name cannot be empty" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
}

void manageCategories(Database& db) {
    std::cout << "\n=== Manage Categories ===" << std::endl;
    
    while (true) {
        std::vector<std::string> options = {
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
                    std::cout << "\nAll Categories (" << categories.size() << "):" << std::endl;
                    for (const auto& category : categories) {
                        std::cout << "ID: " << category.id << ", Name: " 
                                 << category.name << std::endl;
                    }
                } else {
                    std::cout << "No categories found." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error retrieving categories: " << e.what() << std::endl;
            }
        } else if (choice == 2) {
            // Add new category
            try {
                std::string name = Utils::getUserInput("Enter new category name: ");
                if (Utils::validateNonEmpty(name)) {
                    int category_id = Models::createCategory(db, name);
                    std::cout << "Category '" << name << "' created successfully with ID: " 
                             << category_id << std::endl;
                } else {
                    std::cout << "Error: Category name cannot be empty" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
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
    std::cout << "Welcome to the Expense Tracking System!" << std::endl;
    std::cout << "Initializing database..." << std::endl;
    
    // C++ FEATURE: Stack allocation with RAII
    // Database destructor will automatically be called when main() exits
    // CONTRAST WITH PYTHON: Python uses 'with' statement or garbage collection
    Database db("expenses.db");
    
    try {
        if (!db.initialize()) {
            std::cerr << "Failed to initialize database" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize database: " << e.what() << std::endl;
        return 1;
    }
    
    // Main application loop
    while (true) {
        try {
            displayMainMenu();
            int choice = Utils::getMenuChoice(9);
            
            if (choice == 0) {
                std::cout << "\nThank you for using the Expense Tracking System!" << std::endl;
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
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
            }
            
        } catch (const std::exception& e) {
            std::cerr << "\nUnexpected error: " << e.what() << std::endl;
            std::cout << "Please try again or contact support if the problem persists." << std::endl;
        }
    }
    
    // C++ FEATURE: Explicit return code (0 = success)
    // Database destructor is automatically called here
    return 0;
}

