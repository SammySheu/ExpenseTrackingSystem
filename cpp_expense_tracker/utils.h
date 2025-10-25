/**
 * utils.h
 * Utility functions for validation and formatting
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Function overloading
 * - String manipulation with std::string
 * - Reference parameters for output
 * - Static class organization
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "expense.h"

class Utils {
public:
    // Validation functions
    static bool validateDate(const std::string& date);
    static bool validateAmount(double amount);
    static bool validateNonEmpty(const std::string& value);
    
    // Parsing functions
    static bool parseDate(const std::string& input, std::string& output);
    static bool parseAmount(const std::string& input, double& output);
    
    // Formatting functions
    static std::string formatCurrency(double amount);
    static std::string formatExpenseOutput(const std::vector<Expense>& expenses);
    static std::string formatSummaryOutput(const ExpenseSummary& summary);
    
    // Input functions
    static std::string getUserInput(const std::string& prompt);
    static bool getDateRangeInput(std::string& min_date, std::string& max_date);
    static bool getAmountRangeInput(double& min_amount, double& max_amount);
    
    // Menu functions
    static void displayMenu(const std::vector<std::string>& options, 
                          const std::string& title = "Menu");
    static int getMenuChoice(int max_choice);
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

#endif // UTILS_H

