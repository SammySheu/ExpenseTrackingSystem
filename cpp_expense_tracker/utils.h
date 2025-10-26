/**
 * utils.h
 * Utility functions for validation and formatting
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Function overloading
 * - String manipulation with string
 * - Reference parameters for output
 * - Static class organization
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "expense.h"

using namespace std;

class Utils {
public:
    // Validation functions
    static bool validateDate(const string& date);
    static bool validateAmount(double amount);
    static bool validateNonEmpty(const string& value);
    
    // Parsing functions
    static bool parseDate(const string& input, string& output);
    static bool parseAmount(const string& input, double& output);
    
    // Formatting functions
    static string formatCurrency(double amount);
    static string formatExpenseOutput(const vector<Expense>& expenses);
    static string formatSummaryOutput(const ExpenseSummary& summary);
    
    // Input functions
    static string getUserInput(const string& prompt);
    static bool getDateRangeInput(string& min_date, string& max_date);
    static bool getAmountRangeInput(double& min_amount, double& max_amount);
    
    // Menu functions
    static void displayMenu(const vector<string>& options, 
                          const string& title = "Menu");
    static int getMenuChoice(int max_choice);
    
    // String utilities
    static string trim(const string& str);
    static string toLower(const string& str);
};

#endif // UTILS_H

