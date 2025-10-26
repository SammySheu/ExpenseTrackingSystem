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

using namespace std;

/**
 * C++ FEATURE: Exception handling with try/catch
 * CONTRAST WITH PYTHON: Similar exception mechanism but different syntax
 * Python uses 'raise' and 'except', C++ uses 'throw' and 'catch'
 */
int ExpenseOperations::recordExpense(Database& db, const string& date,
                                    const string& category, const string& title,
                                    double amount, const string& user_name) {
    // Validate inputs
    if (!Utils::validateDate(date)) {
        throw invalid_argument("Invalid date format");
    }
    if (!Utils::validateAmount(amount)) {
        throw invalid_argument("Amount must be positive");
    }
    if (!Utils::validateNonEmpty(title)) {
        throw invalid_argument("Title cannot be empty");
    }
    if (!Utils::validateNonEmpty(user_name)) {
        throw invalid_argument("User name cannot be empty");
    }
    if (!Utils::validateNonEmpty(category)) {
        throw invalid_argument("Category cannot be empty");
    }
    
    // Get or create user
    int user_id = 0;
    auto user_opt = Models::getUserByName(db, user_name);
    
    if (user_opt.has_value()) {
        // C++ FEATURE: optional value extraction
        user_id = user_opt.value().id;
    } else {
        try {
            user_id = Models::createUser(db, user_name);
            cout << "Created new user: " << user_name << endl;
        } catch (const exception& e) {
            throw runtime_error("Failed to create user: " + string(e.what()));
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
            cout << "Created new category: " << category << endl;
        } catch (const exception& e) {
            throw runtime_error("Failed to create category: " + string(e.what()));
        }
    }
    
    // Insert expense
    try {
        int expense_id = Models::insertExpense(db, date, category_id, title, amount, user_id);
        cout << "Expense recorded successfully with ID: " << expense_id << endl;
        return expense_id;
    } catch (const exception& e) {
        throw runtime_error("Failed to record expense: " + string(e.what()));
    }
}

vector<Expense> ExpenseOperations::viewExpensesByDate(Database& db,
                                                          const string& min_date,
                                                          const string& max_date) {
    try {
        // C++ FEATURE: Pointer to optional parameters
        const string* min_ptr = min_date.empty() ? nullptr : &min_date;
        const string* max_ptr = max_date.empty() ? nullptr : &max_date;
        
        return Models::fetchExpensesByFilters(db, min_ptr, max_ptr);
    } catch (const exception& e) {
        cerr << "Error filtering expenses by date: " << e.what() << endl;
        return vector<Expense>();
    }
}

vector<Expense> ExpenseOperations::viewExpensesByAmount(Database& db,
                                                            double min_amount,
                                                            double max_amount) {
    try {
        const double* min_ptr = (min_amount > 0.0) ? &min_amount : nullptr;
        const double* max_ptr = (max_amount > 0.0) ? &max_amount : nullptr;
        
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, min_ptr, max_ptr);
    } catch (const exception& e) {
        cerr << "Error filtering expenses by amount: " << e.what() << endl;
        return vector<Expense>();
    }
}

vector<Expense> ExpenseOperations::viewExpensesByCategory(
    Database& db, const vector<string>& categories) {
    
    if (categories.empty()) {
        return vector<Expense>();
    }
    
    try {
        // Get all categories and build lookup map
        auto all_categories = Models::getAllCategories(db);
        
        // C++ FEATURE: STL map for lookups
        map<string, int> category_lookup;
        for (const auto& cat : all_categories) {
            category_lookup[Utils::toLower(cat.name)] = cat.id;
        }
        
        // Find category IDs
        vector<int> category_ids;
        for (const auto& cat_name : categories) {
            string lower_name = Utils::toLower(cat_name);
            auto it = category_lookup.find(lower_name);
            if (it != category_lookup.end()) {
                category_ids.push_back(it->second);
            }
        }
        
        if (category_ids.empty()) {
            cout << "No valid categories found." << endl;
            return vector<Expense>();
        }
        
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, nullptr, nullptr, &category_ids);
    } catch (const exception& e) {
        cerr << "Error filtering expenses by category: " << e.what() << endl;
        return vector<Expense>();
    }
}

vector<Expense> ExpenseOperations::viewExpensesByUser(Database& db, int user_id) {
    if (user_id <= 0) {
        throw invalid_argument("User ID must be positive");
    }
    
    try {
        return Models::fetchExpensesByFilters(db, nullptr, nullptr, nullptr, nullptr, nullptr, &user_id);
    } catch (const exception& e) {
        cerr << "Error filtering expenses by user: " << e.what() << endl;
        return vector<Expense>();
    }
}

vector<Expense> ExpenseOperations::viewAllExpenses(Database& db) {
    try {
        return Models::fetchExpensesByFilters(db);
    } catch (const exception& e) {
        cerr << "Error retrieving all expenses: " << e.what() << endl;
        return vector<Expense>();
    }
}

/**
 * C++ FEATURE: STL map and algorithms for aggregation
 * CONTRAST WITH PYTHON: Python's dict.get() and list comprehensions are simpler
 * C++ requires more verbose iteration and map operations
 */
ExpenseSummary ExpenseOperations::calculateSummary(Database& db,
                                                  const vector<Expense>* expenses) {
    ExpenseSummary summary;
    
    try {
        vector<Expense> exp_list;
        
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
    } catch (const exception& e) {
        cerr << "Error calculating summary: " << e.what() << endl;
        return summary;
    }
}

vector<Expense> ExpenseOperations::getExpensesByDateRange(Database& db) {
    cout << "\n=== Filter Expenses by Date Range ===" << endl;
    
    string min_date, max_date;
    if (!Utils::getDateRangeInput(min_date, max_date)) {
        return vector<Expense>();
    }
    
    if (min_date.empty() && max_date.empty()) {
        cout << "No date range specified. Showing all expenses." << endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByDate(db, min_date, max_date);
}

vector<Expense> ExpenseOperations::getExpensesByAmountRange(Database& db) {
    cout << "\n=== Filter Expenses by Amount Range ===" << endl;
    
    double min_amount = 0.0, max_amount = 0.0;
    if (!Utils::getAmountRangeInput(min_amount, max_amount)) {
        return vector<Expense>();
    }
    
    if (min_amount == 0.0 && max_amount == 0.0) {
        cout << "No amount range specified. Showing all expenses." << endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByAmount(db, min_amount, max_amount);
}

vector<Expense> ExpenseOperations::getExpensesByCategory(Database& db) {
    cout << "\n=== Filter Expenses by Category ===" << endl;
    
    // Show available categories
    auto categories = Models::getAllCategories(db);
    if (categories.empty()) {
        cout << "No categories available." << endl;
        return vector<Expense>();
    }
    
    cout << "Available categories:" << endl;
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << (i + 1) << ". " << categories[i].name << endl;
    }
    
    // Get user selections
    vector<string> selected_categories;
    
    while (true) {
        string choice = Utils::getUserInput("\nEnter category number (or 'done' to finish): ");
        
        if (Utils::toLower(choice) == "done") {
            break;
        }
        
        try {
            int choice_num = stoi(choice);
            if (choice_num >= 1 && choice_num <= static_cast<int>(categories.size())) {
                string selected = categories[choice_num - 1].name;
                
                // Check if already selected
                if (find(selected_categories.begin(), selected_categories.end(), selected) 
                    == selected_categories.end()) {
                    selected_categories.push_back(selected);
                    cout << "Added category: " << selected << endl;
                } else {
                    cout << "Category " << selected << " already selected" << endl;
                }
            } else {
                cout << "Please enter a number between 1 and " << categories.size() << endl;
            }
        } catch (const exception&) {
            cout << "Please enter a valid number or 'done'" << endl;
        }
    }
    
    if (selected_categories.empty()) {
        cout << "No categories selected. Showing all expenses." << endl;
        return viewAllExpenses(db);
    }
    
    return viewExpensesByCategory(db, selected_categories);
}

vector<Expense> ExpenseOperations::getExpensesByUser(Database& db) {
    cout << "\n=== Filter Expenses by User ===" << endl;
    
    // Show available users
    auto users = Models::getAllUsers(db);
    if (users.empty()) {
        cout << "No users available." << endl;
        return vector<Expense>();
    }
    
    cout << "Available users:" << endl;
    for (size_t i = 0; i < users.size(); ++i) {
        cout << (i + 1) << ". " << users[i].name << endl;
    }
    
    // Get user selection
    try {
        string choice = Utils::getUserInput("\nEnter user number (1-" + 
                                                 to_string(users.size()) + "): ");
        int choice_num = stoi(choice);
        
        if (choice_num >= 1 && choice_num <= static_cast<int>(users.size())) {
            return viewExpensesByUser(db, users[choice_num - 1].id);
        } else {
            cout << "Please enter a number between 1 and " << users.size() << endl;
            return vector<Expense>();
        }
    } catch (const exception&) {
        cout << "Please enter a valid number" << endl;
        return vector<Expense>();
    }
}

