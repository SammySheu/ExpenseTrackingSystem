/**
 * expense_operations.cpp
 * Implementation of business logic
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Exception handling (throw/catch vs Python's raise/except)
 * - STL algorithms and iterators
 * - Map operations for aggregation
 */

#include "expense_operations.h"
#include "models.h"
#include "utils.h"
#include <iostream>
#include <algorithm>

/**
 * C++ FEATURE: Exception handling with try/catch
 * CONTRAST WITH PYTHON: Similar exception mechanism but different syntax
 * Python uses 'raise' and 'except', C++ uses 'throw' and 'catch'
 */
int ExpenseOperations::recordExpense(Database& db, const std::string& date,
                                    const std::string& category, const std::string& title,
                                    double amount, const std::string& user_name) {
    // Validate inputs
    if (!Utils::validateDate(date)) {
        throw std::invalid_argument("Invalid date format");
    }
    if (!Utils::validateAmount(amount)) {
        throw std::invalid_argument("Amount must be positive");
    }
    if (!Utils::validateNonEmpty(title)) {
        throw std::invalid_argument("Title cannot be empty");
    }
    if (!Utils::validateNonEmpty(user_name)) {
        throw std::invalid_argument("User name cannot be empty");
    }
    if (!Utils::validateNonEmpty(category)) {
        throw std::invalid_argument("Category cannot be empty");
    }
    
    // Get or create user
    int user_id = 0;
    auto user_opt = Models::getUserByName(db, user_name);
    
    if (user_opt.has_value()) {
        // C++ FEATURE: std::optional value extraction
        user_id = user_opt.value().id;
    } else {
        try {
            user_id = Models::createUser(db, user_name);
            std::cout << "Created new user: " << user_name << std::endl;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to create user: " + std::string(e.what()));
        }
    }
    
    // Get or create category
    int category_id = 0;
    auto category_opt = Models::getCategoryByName(db, category);
    
    if (category_opt.has_value()) {
        category_id = category_opt.value().id;
    } else {
        try {
            category_id = Models::createCategory(db, category);
            std::cout << "Created new category: " << category << std::endl;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to create category: " + std::string(e.what()));
        }
    }
    
    // Insert expense
    try {
        int expense_id = Models::insertExpense(db, date, category_id, title, amount, user_id);
        std::cout << "Expense recorded successfully with ID: " << expense_id << std::endl;
        return expense_id;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to record expense: " + std::string(e.what()));
    }
}

std::vector<Expense> ExpenseOperations::viewExpensesByDate(Database& db,
                                                          const std::string& min_date,
                                                          const std::string& max_date) {
    try {
        // C++ FEATURE: Pointer to optional parameters
        const std::string* min_ptr = min_date.empty() ? nullptr : &min_date;
        const std::string* max_ptr = max_date.empty() ? nullptr : &max_date;
        
        return Models::fetchExpensesByFilters(db, min_ptr, max_ptr);
    } catch (const std::exception& e) {
        std::cerr << "Error filtering expenses by date: " << e.what() << std::endl;
        return std::vector<Expense>();
    }
}

std::vector<Expense> ExpenseOperations::viewExpensesByAmount(Database& db,
                                                            double min_amount,
                                                            double max_amount) {
    try {
        const double* min_ptr = (min_amount > 0.0) ? &min_amount : nullptr;
        const double* max_ptr = (max_amount > 0.0) ? &max_amount : nullptr;
        
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, min_ptr, max_ptr);
    } catch (const std::exception& e) {
        std::cerr << "Error filtering expenses by amount: " << e.what() << std::endl;
        return std::vector<Expense>();
    }
}

std::vector<Expense> ExpenseOperations::viewExpensesByCategory(
    Database& db, const std::vector<std::string>& categories) {
    
    if (categories.empty()) {
        return std::vector<Expense>();
    }
    
    try {
        // Get all categories and build lookup map
        auto all_categories = Models::getAllCategories(db);
        
        // C++ FEATURE: STL map for lookups
        std::map<std::string, int> category_lookup;
        for (const auto& cat : all_categories) {
            category_lookup[Utils::toLower(cat.name)] = cat.id;
        }
        
        // Find category IDs
        std::vector<int> category_ids;
        for (const auto& cat_name : categories) {
            std::string lower_name = Utils::toLower(cat_name);
            auto it = category_lookup.find(lower_name);
            if (it != category_lookup.end()) {
                category_ids.push_back(it->second);
            }
        }
        
        if (category_ids.empty()) {
            std::cout << "No valid categories found." << std::endl;
            return std::vector<Expense>();
        }
        
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, nullptr, nullptr, &category_ids);
    } catch (const std::exception& e) {
        std::cerr << "Error filtering expenses by category: " << e.what() << std::endl;
        return std::vector<Expense>();
    }
}

std::vector<Expense> ExpenseOperations::viewExpensesByUser(Database& db, int user_id) {
    if (user_id <= 0) {
        throw std::invalid_argument("User ID must be positive");
    }
    
    try {
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, nullptr, nullptr, nullptr, &user_id);
    } catch (const std::exception& e) {
        std::cerr << "Error filtering expenses by user: " << e.what() << std::endl;
        return std::vector<Expense>();
    }
}

std::vector<Expense> ExpenseOperations::viewAllExpenses(Database& db) {
    try {
        return Models::fetchExpensesByFilters(db);
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving all expenses: " << e.what() << std::endl;
        return std::vector<Expense>();
    }
}

/**
 * C++ FEATURE: STL map and algorithms for aggregation
 * CONTRAST WITH PYTHON: Python's dict.get() and list comprehensions are simpler
 * C++ requires more verbose iteration and map operations
 */
ExpenseSummary ExpenseOperations::calculateSummary(Database& db,
                                                  const std::vector<Expense>* expenses) {
    ExpenseSummary summary;
    
    try {
        std::vector<Expense> exp_list;
        
        if (expenses == nullptr) {
            exp_list = viewAllExpenses(db);
        } else {
            exp_list = *expenses;
        }
        
        if (exp_list.empty()) {
            return summary;
        }
        
        // Calculate totals
        summary.count = exp_list.size();
        
        // C++ FEATURE: STL algorithm for aggregation
        // CONTRAST: Python's sum() with generator expression is simpler
        summary.total = 0.0;
        for (const auto& expense : exp_list) {
            summary.total += expense.amount;
        }
        
        // Group by category
        for (const auto& expense : exp_list) {
            // C++ FEATURE: Map operator[] for auto-insertion
            summary.by_category[expense.category_name] += expense.amount;
        }
        
        // Group by user
        for (const auto& expense : exp_list) {
            summary.by_user[expense.user_name] += expense.amount;
            summary.user_expenses[expense.user_name].push_back(expense);
        }
        
        return summary;
    } catch (const std::exception& e) {
        std::cerr << "Error calculating summary: " << e.what() << std::endl;
        return summary;
    }
}

std::vector<Expense> ExpenseOperations::getExpensesByDateRange(Database& db) {
    std::cout << "\n=== Filter Expenses by Date Range ===" << std::endl;
    
    std::string min_date, max_date;
    if (!Utils::getDateRangeInput(min_date, max_date)) {
        return std::vector<Expense>();
    }
    
    if (min_date.empty() && max_date.empty()) {
        std::cout << "No date range specified. Showing all expenses." << std::endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByDate(db, min_date, max_date);
}

std::vector<Expense> ExpenseOperations::getExpensesByAmountRange(Database& db) {
    std::cout << "\n=== Filter Expenses by Amount Range ===" << std::endl;
    
    double min_amount = 0.0, max_amount = 0.0;
    if (!Utils::getAmountRangeInput(min_amount, max_amount)) {
        return std::vector<Expense>();
    }
    
    if (min_amount == 0.0 && max_amount == 0.0) {
        std::cout << "No amount range specified. Showing all expenses." << std::endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByAmount(db, min_amount, max_amount);
}

std::vector<Expense> ExpenseOperations::getExpensesByCategory(Database& db) {
    std::cout << "\n=== Filter Expenses by Category ===" << std::endl;
    
    // Show available categories
    auto categories = Models::getAllCategories(db);
    if (categories.empty()) {
        std::cout << "No categories available." << std::endl;
        return std::vector<Expense>();
    }
    
    std::cout << "Available categories:" << std::endl;
    for (size_t i = 0; i < categories.size(); ++i) {
        std::cout << (i + 1) << ". " << categories[i].name << std::endl;
    }
    
    // Get user selections
    std::vector<std::string> selected_categories;
    
    while (true) {
        std::string choice = Utils::getUserInput("\nEnter category number (or 'done' to finish): ");
        
        if (Utils::toLower(choice) == "done") {
            break;
        }
        
        try {
            int choice_num = std::stoi(choice);
            if (choice_num >= 1 && choice_num <= static_cast<int>(categories.size())) {
                std::string selected = categories[choice_num - 1].name;
                
                // Check if already selected
                if (std::find(selected_categories.begin(), selected_categories.end(), selected) 
                    == selected_categories.end()) {
                    selected_categories.push_back(selected);
                    std::cout << "Added category: " << selected << std::endl;
                } else {
                    std::cout << "Category " << selected << " already selected" << std::endl;
                }
            } else {
                std::cout << "Please enter a number between 1 and " << categories.size() << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "Please enter a valid number or 'done'" << std::endl;
        }
    }
    
    if (selected_categories.empty()) {
        std::cout << "No categories selected. Showing all expenses." << std::endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByCategory(db, selected_categories);
}

std::vector<Expense> ExpenseOperations::getExpensesByUser(Database& db) {
    std::cout << "\n=== Filter Expenses by User ===" << std::endl;
    
    // Show available users
    auto users = Models::getAllUsers(db);
    if (users.empty()) {
        std::cout << "No users available." << std::endl;
        return std::vector<Expense>();
    }
    
    std::cout << "Available users:" << std::endl;
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << (i + 1) << ". " << users[i].name << std::endl;
    }
    
    // Get user selection
    try {
        std::string choice = Utils::getUserInput("\nEnter user number (1-" + 
                                                 std::to_string(users.size()) + "): ");
        int choice_num = std::stoi(choice);
        
        if (choice_num >= 1 && choice_num <= static_cast<int>(users.size())) {
            return viewExpensesByUser(db, users[choice_num - 1].id);
        } else {
            std::cout << "Please enter a number between 1 and " << users.size() << std::endl;
            return std::vector<Expense>();
        }
    } catch (const std::exception&) {
        std::cout << "Please enter a valid number" << std::endl;
        return std::vector<Expense>();
    }
}

