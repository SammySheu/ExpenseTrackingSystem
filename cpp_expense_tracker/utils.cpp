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

/**
 * C++ FEATURE: Regular expressions (similar to Python's re module)
 * CONTRAST: C++ regex is more verbose than Python's
 */
bool Utils::validateDate(const std::string& date) {
    // Check format YYYY-MM-DD
    std::regex date_pattern(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!std::regex_match(date, date_pattern)) {
        return false;
    }
    
    // Basic date validation (simplified)
    int year, month, day;
    char dash1, dash2;
    std::stringstream ss(date);
    ss >> year >> dash1 >> month >> dash2 >> day;
    
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    return true;
}

bool Utils::validateAmount(double amount) {
    return amount > 0.0;
}

bool Utils::validateNonEmpty(const std::string& value) {
    return !trim(value).empty();
}

/**
 * C++ FEATURE: Reference parameters for output
 * output parameter is modified to return the parsed value
 * 
 * CONTRAST WITH PYTHON: Python would return tuple or throw exception
 * C++ uses bool return + reference parameter pattern
 */
bool Utils::parseDate(const std::string& input, std::string& output) {
    std::string trimmed = trim(input);
    if (validateDate(trimmed)) {
        output = trimmed;
        return true;
    }
    return false;
}

bool Utils::parseAmount(const std::string& input, double& output) {
    try {
        // C++ FEATURE: std::stod for string to double conversion
        // CONTRAST: Python's float() is simpler
        output = std::stod(trim(input));
        return validateAmount(output);
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * C++ FEATURE: Stream manipulation for formatting
 * Using std::stringstream and std::setprecision
 * 
 * CONTRAST WITH PYTHON: Python's f-strings are much simpler
 */
std::string Utils::formatCurrency(double amount) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "$" << amount;
    return ss.str();
}

/**
 * C++ FEATURE: Manual string building with loops
 * More verbose than Python's string formatting
 */
std::string Utils::formatExpenseOutput(const std::vector<Expense>& expenses) {
    if (expenses.empty()) {
        return "No expenses found.";
    }
    
    std::stringstream output;
    
    // Calculate column widths
    size_t id_width = 3;
    size_t date_width = 10;
    size_t title_width = 15;
    size_t amount_width = 10;
    size_t category_width = 10;
    size_t user_width = 10;
    
    for (const auto& exp : expenses) {
        id_width = std::max(id_width, std::to_string(exp.id).length());
        date_width = std::max(date_width, exp.date.length());
        title_width = std::max(title_width, exp.title.length());
        amount_width = std::max(amount_width, formatCurrency(exp.amount).length());
        category_width = std::max(category_width, exp.category_name.length());
        user_width = std::max(user_width, exp.user_name.length());
    }
    
    // Create header
    output << std::left
           << std::setw(id_width) << "ID" << " | "
           << std::setw(date_width) << "Date" << " | "
           << std::setw(title_width) << "Title" << " | "
           << std::setw(amount_width) << "Amount" << " | "
           << std::setw(category_width) << "Category" << " | "
           << std::setw(user_width) << "User" << "\n";
    
    // Separator
    output << std::string(id_width + date_width + title_width + 
                         amount_width + category_width + user_width + 15, '-') << "\n";
    
    // Data rows
    for (const auto& exp : expenses) {
        output << std::left
               << std::setw(id_width) << exp.id << " | "
               << std::setw(date_width) << exp.date << " | "
               << std::setw(title_width) << exp.title << " | "
               << std::setw(amount_width) << formatCurrency(exp.amount) << " | "
               << std::setw(category_width) << exp.category_name << " | "
               << std::setw(user_width) << exp.user_name << "\n";
    }
    
    return output.str();
}

std::string Utils::formatSummaryOutput(const ExpenseSummary& summary) {
    std::stringstream output;
    
    output << "=== EXPENSE SUMMARY ===\n\n";
    
    // Show each user's expenses
    if (!summary.user_expenses.empty()) {
        output << "EXPENSES BY USER:\n";
        output << std::string(80, '=') << "\n";
        
        for (const auto& [user, expenses] : summary.user_expenses) {
            output << "\n" << user << "'s Expenses:\n";
            output << std::string(80, '-') << "\n";
            
            if (!expenses.empty()) {
                // Format expenses for this user
                size_t date_width = 10;
                size_t title_width = 15;
                size_t amount_width = 10;
                size_t category_width = 10;
                
                for (const auto& exp : expenses) {
                    date_width = std::max(date_width, exp.date.length());
                    title_width = std::max(title_width, exp.title.length());
                    amount_width = std::max(amount_width, formatCurrency(exp.amount).length());
                    category_width = std::max(category_width, exp.category_name.length());
                }
                
                // Header
                output << std::left
                       << std::setw(date_width) << "Date" << " | "
                       << std::setw(title_width) << "Title" << " | "
                       << std::setw(amount_width) << "Amount" << " | "
                       << std::setw(category_width) << "Category" << "\n";
                
                output << std::string(date_width + title_width + amount_width + category_width + 9, '-') << "\n";
                
                // Expenses
                for (const auto& exp : expenses) {
                    output << std::left
                           << std::setw(date_width) << exp.date << " | "
                           << std::setw(title_width) << exp.title << " | "
                           << std::setw(amount_width) << formatCurrency(exp.amount) << " | "
                           << std::setw(category_width) << exp.category_name << "\n";
                }
                
                // Total for user
                double user_total = summary.by_user.at(user);
                output << std::string(date_width + title_width + amount_width + category_width + 9, '-') << "\n";
                output << std::left
                       << std::setw(date_width) << "TOTAL" << " | "
                       << std::setw(title_width) << "" << " | "
                       << std::setw(amount_width) << formatCurrency(user_total) << " | "
                       << expenses.size() << " expense(s)\n";
            }
            output << "\n";
        }
        
        output << std::string(80, '=') << "\n\n";
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
        std::vector<std::pair<std::string, double>> sorted_categories(
            summary.by_category.begin(), summary.by_category.end()
        );
        
        std::sort(sorted_categories.begin(), sorted_categories.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (const auto& [category, amount] : sorted_categories) {
            double percentage = (amount / summary.total) * 100.0;
            output << "  " << category << ": " << formatCurrency(amount)
                   << " (" << std::fixed << std::setprecision(1) << percentage << "%)\n";
        }
    }
    
    return output.str();
}

std::string Utils::getUserInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return trim(input);
}

bool Utils::getDateRangeInput(std::string& min_date, std::string& max_date) {
    std::cout << "\nEnter date range (YYYY-MM-DD format, leave empty to skip):\n";
    
    std::string min_input = getUserInput("Start date (min): ");
    std::string max_input = getUserInput("End date (max): ");
    
    bool has_min = false, has_max = false;
    
    if (!min_input.empty()) {
        if (!parseDate(min_input, min_date)) {
            std::cout << "Invalid start date format\n";
            return false;
        }
        has_min = true;
    }
    
    if (!max_input.empty()) {
        if (!parseDate(max_input, max_date)) {
            std::cout << "Invalid end date format\n";
            return false;
        }
        has_max = true;
    }
    
    if (has_min && has_max && min_date > max_date) {
        std::cout << "Start date cannot be after end date\n";
        return false;
    }
    
    return true;
}

bool Utils::getAmountRangeInput(double& min_amount, double& max_amount) {
    std::cout << "\nEnter amount range (leave empty to skip):\n";
    
    std::string min_input = getUserInput("Minimum amount: ");
    std::string max_input = getUserInput("Maximum amount: ");
    
    bool has_min = false, has_max = false;
    
    if (!min_input.empty()) {
        if (!parseAmount(min_input, min_amount)) {
            std::cout << "Invalid minimum amount\n";
            return false;
        }
        has_min = true;
    }
    
    if (!max_input.empty()) {
        if (!parseAmount(max_input, max_amount)) {
            std::cout << "Invalid maximum amount\n";
            return false;
        }
        has_max = true;
    }
    
    if (has_min && has_max && min_amount > max_amount) {
        std::cout << "Minimum amount cannot be greater than maximum amount\n";
        return false;
    }
    
    return true;
}

void Utils::displayMenu(const std::vector<std::string>& options, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << (i + 1) << ". " << options[i] << "\n";
    }
    std::cout << "0. Exit\n";
}

int Utils::getMenuChoice(int max_choice) {
    while (true) {
        std::string input = getUserInput("\nEnter your choice (0-" + 
                                        std::to_string(max_choice) + "): ");
        try {
            int choice = std::stoi(input);
            if (choice >= 0 && choice <= max_choice) {
                return choice;
            }
            std::cout << "Please enter a number between 0 and " << max_choice << "\n";
        } catch (const std::exception&) {
            std::cout << "Please enter a valid number\n";
        }
    }
}

/**
 * C++ FEATURE: Manual string trimming
 * CONTRAST WITH PYTHON: Python's str.strip() is built-in and simpler
 */
std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Utils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

