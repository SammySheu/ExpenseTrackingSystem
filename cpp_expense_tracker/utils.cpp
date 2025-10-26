/**
 * utils.cpp
 * Implementation of utility functions
 * 
 * C++ FEATURES DEMONSTRATED:
 * - String manipulation (more verbose than Python)
 * - Stream operations (stringstream, iostream)
 * - Manual string processing vs Python's built-in functions
 * - Reference parameters for output values
 */

#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cctype>

using namespace std;

/**
 * C++ FEATURE: Regular expressions (similar to Python's re module)
 * CONTRAST: C++ regex is more verbose than Python's
 */
bool Utils::validateDate(const string& date) {
    // Check format YYYY-MM-DD
    regex date_pattern(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!regex_match(date, date_pattern)) {
        return false;
    }
    
    // Basic date validation with proper day-per-month checking
    int year, month, day;
    char dash1, dash2;
    stringstream ss(date);
    ss >> year >> dash1 >> month >> dash2 >> day;
    
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;
    
    // Days per month
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    bool is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap && month == 2) {
        days_in_month[1] = 29;
    }
    
    if (day > days_in_month[month - 1]) return false;
    
    return true;
}

bool Utils::validateAmount(double amount) {
    return amount > 0.0;
}

bool Utils::validateNonEmpty(const string& value) {
    return !trim(value).empty();
}

/**
 * C++ FEATURE: Reference parameters for output
 * output parameter is modified to return the parsed value
 * 
 * CONTRAST WITH PYTHON: Python would return tuple or throw exception
 * C++ uses bool return + reference parameter pattern
 */
bool Utils::parseDate(const string& input, string& output) {
    string trimmed = trim(input);
    if (validateDate(trimmed)) {
        output = trimmed;
        return true;
    }
    return false;
}

bool Utils::parseAmount(const string& input, double& output) {
    try {
        // C++ FEATURE: stod for string to double conversion
        // CONTRAST: Python's float() is simpler
        output = stod(trim(input));
        return validateAmount(output);
    } catch (const exception&) {
        return false;
    }
}

/**
 * C++ FEATURE: Stream manipulation for formatting
 * Using stringstream and setprecision
 * 
 * CONTRAST WITH PYTHON: Python's f-strings are much simpler
 */
string Utils::formatCurrency(double amount) {
    stringstream ss;
    ss << fixed << setprecision(2) << "$" << amount;
    return ss.str();
}

/**
 * C++ FEATURE: Manual string building with loops
 * More verbose than Python's string formatting
 */
string Utils::formatExpenseOutput(const vector<Expense>& expenses) {
    if (expenses.empty()) {
        return "No expenses found.";
    }
    
    stringstream output;
    
    // Calculate column widths
    size_t id_width = 3;
    size_t date_width = 10;
    size_t title_width = 15;
    size_t amount_width = 10;
    size_t category_width = 10;
    size_t user_width = 10;
    
    for (const auto& exp : expenses) {
        id_width = max(id_width, to_string(exp.id).length());
        date_width = max(date_width, exp.date.length());
        title_width = max(title_width, exp.title.length());
        amount_width = max(amount_width, formatCurrency(exp.amount).length());
        category_width = max(category_width, exp.category_name.length());
        user_width = max(user_width, exp.user_name.length());
    }
    
    // Create header
    output << left
           << setw(id_width) << "ID" << " | "
           << setw(date_width) << "Date" << " | "
           << setw(title_width) << "Title" << " | "
           << setw(amount_width) << "Amount" << " | "
           << setw(category_width) << "Category" << " | "
           << setw(user_width) << "User" << "\n";
    
    // Separator
    output << string(id_width + date_width + title_width + 
                         amount_width + category_width + user_width + 15, '-') << "\n";
    
    // Data rows
    for (const auto& exp : expenses) {
        output << left
               << setw(id_width) << exp.id << " | "
               << setw(date_width) << exp.date << " | "
               << setw(title_width) << exp.title << " | "
               << setw(amount_width) << formatCurrency(exp.amount) << " | "
               << setw(category_width) << exp.category_name << " | "
               << setw(user_width) << exp.user_name << "\n";
    }
    
    return output.str();
}

string Utils::formatSummaryOutput(const ExpenseSummary& summary) {
    stringstream output;
    
    output << "=== EXPENSE SUMMARY ===\n\n";
    
    // Show each user's expenses
    if (!summary.user_expenses.empty()) {
        output << "EXPENSES BY USER:\n";
        output << string(80, '=') << "\n";
        
        for (const auto& [user, expenses] : summary.user_expenses) {
            output << "\n" << user << "'s Expenses:\n";
            output << string(80, '-') << "\n";
            
            if (!expenses.empty()) {
                // Format expenses for this user
                size_t date_width = 10;
                size_t title_width = 15;
                size_t amount_width = 10;
                size_t category_width = 10;
                
                for (const auto& exp : expenses) {
                    date_width = max(date_width, exp.date.length());
                    title_width = max(title_width, exp.title.length());
                    amount_width = max(amount_width, formatCurrency(exp.amount).length());
                    category_width = max(category_width, exp.category_name.length());
                }
                
                // Header
                output << left
                       << setw(date_width) << "Date" << " | "
                       << setw(title_width) << "Title" << " | "
                       << setw(amount_width) << "Amount" << " | "
                       << setw(category_width) << "Category" << "\n";
                
                output << string(date_width + title_width + amount_width + category_width + 9, '-') << "\n";
                
                // Expenses
                for (const auto& exp : expenses) {
                    output << left
                           << setw(date_width) << exp.date << " | "
                           << setw(title_width) << exp.title << " | "
                           << setw(amount_width) << formatCurrency(exp.amount) << " | "
                           << setw(category_width) << exp.category_name << "\n";
                }
                
                // Total for user
                double user_total = summary.by_user.at(user);
                output << string(date_width + title_width + amount_width + category_width + 9, '-') << "\n";
                output << left
                       << setw(date_width) << "TOTAL" << " | "
                       << setw(title_width) << "" << " | "
                       << setw(amount_width) << formatCurrency(user_total) << " | "
                       << expenses.size() << " expense(s)\n";
            }
            output << "\n";
        }
        
        output << string(80, '=') << "\n\n";
    }
    
    // Overall summary
    output << "OVERALL SUMMARY:\n";
    output << "Total Expenses: " << formatCurrency(summary.total) << "\n";
    output << "Number of Expenses: " << summary.count << "\n\n";
    
    // Category breakdown
    if (!summary.by_category.empty() && summary.total > 0) {
        output << "CATEGORY BREAKDOWN (with Percentages):\n";
        
        // C++ FEATURE: Vector of pairs for sorting
        // CONTRAST: Python's sorted() is simpler
        vector<pair<string, double>> sorted_categories(
            summary.by_category.begin(), summary.by_category.end()
        );
        
        sort(sorted_categories.begin(), sorted_categories.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (const auto& [category, amount] : sorted_categories) {
            double percentage = (amount / summary.total) * 100.0;
            output << "  " << category << ": " << formatCurrency(amount)
                   << " (" << fixed << setprecision(1) << percentage << "%)\n";
        }
    }
    
    return output.str();
}

string Utils::getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return trim(input);
}

bool Utils::getDateRangeInput(string& min_date, string& max_date) {
    cout << "\nEnter date range (YYYY-MM-DD format, leave empty to skip):\n";
    
    string min_input = getUserInput("Start date (min): ");
    string max_input = getUserInput("End date (max): ");
    
    bool has_min = false, has_max = false;
    
    if (!min_input.empty()) {
        if (!parseDate(min_input, min_date)) {
            cout << "Invalid start date format\n";
            return false;
        }
        has_min = true;
    }
    
    if (!max_input.empty()) {
        if (!parseDate(max_input, max_date)) {
            cout << "Invalid end date format\n";
            return false;
        }
        has_max = true;
    }
    
    if (has_min && has_max && min_date > max_date) {
        cout << "Start date cannot be after end date\n";
        return false;
    }
    
    return true;
}

bool Utils::getAmountRangeInput(double& min_amount, double& max_amount) {
    cout << "\nEnter amount range (leave empty to skip):\n";
    
    string min_input = getUserInput("Minimum amount: ");
    string max_input = getUserInput("Maximum amount: ");
    
    bool has_min = false, has_max = false;
    
    if (!min_input.empty()) {
        if (!parseAmount(min_input, min_amount)) {
            cout << "Invalid minimum amount\n";
            return false;
        }
        has_min = true;
    }
    
    if (!max_input.empty()) {
        if (!parseAmount(max_input, max_amount)) {
            cout << "Invalid maximum amount\n";
            return false;
        }
        has_max = true;
    }
    
    if (has_min && has_max && min_amount > max_amount) {
        cout << "Minimum amount cannot be greater than maximum amount\n";
        return false;
    }
    
    return true;
}

void Utils::displayMenu(const vector<string>& options, const string& title) {
    cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < options.size(); ++i) {
        cout << (i + 1) << ". " << options[i] << "\n";
    }
    cout << "0. Exit\n";
}

int Utils::getMenuChoice(int max_choice) {
    while (true) {
        string input = getUserInput("\nEnter your choice (0-" + 
                                        to_string(max_choice) + "): ");
        try {
            int choice = stoi(input);
            if (choice >= 0 && choice <= max_choice) {
                return choice;
            }
            cout << "Please enter a number between 0 and " << max_choice << "\n";
        } catch (const exception&) {
            cout << "Please enter a valid number\n";
        }
    }
}

/**
 * C++ FEATURE: Manual string trimming
 * CONTRAST WITH PYTHON: Python's str.strip() is built-in and simpler
 */
string Utils::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

string Utils::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return tolower(c); });
    return result;
}

