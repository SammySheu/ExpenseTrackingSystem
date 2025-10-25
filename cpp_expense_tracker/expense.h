/**
 * expense.h
 * Data structures for the Expense Tracking System
 * 
 * C++ FEATURES DEMONSTRATED:
 * - Structs with explicit member types (vs Python's dynamic typing)
 * - Static typing - all types declared at compile time
 * - Value semantics and copy constructors
 * - Header/implementation file separation
 */

#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include <vector>
#include <map>

/**
 * STRUCT vs PYTHON DICT:
 * C++ uses structs with fixed types known at compile time
 * Python uses flexible dictionaries with dynamic typing
 */

struct User {
    int id;
    std::string name;
    
    // C++ FEATURE: Default constructor
    User() : id(0), name("") {}
    
    // C++ FEATURE: Parameterized constructor with initializer list
    User(int id, const std::string& name) : id(id), name(name) {}
};

struct Category {
    int id;
    std::string name;
    
    Category() : id(0), name("") {}
    Category(int id, const std::string& name) : id(id), name(name) {}
};

struct Expense {
    int id;
    std::string date;           // YYYY-MM-DD format
    std::string title;
    double amount;
    std::string created_at;
    int category_id;
    int user_id;
    
    // Additional fields for display (populated from joins)
    std::string category_name;
    std::string user_name;
    
    // C++ FEATURE: Default constructor with member initialization
    Expense() : id(0), date(""), title(""), amount(0.0), 
                created_at(""), category_id(0), user_id(0),
                category_name(""), user_name("") {}
    
    // C++ FEATURE: Parameterized constructor
    Expense(int id, const std::string& date, const std::string& title, 
            double amount, const std::string& created_at,
            int category_id, int user_id)
        : id(id), date(date), title(title), amount(amount),
          created_at(created_at), category_id(category_id), user_id(user_id),
          category_name(""), user_name("") {}
};

/**
 * SUMMARY STRUCTURE
 * C++ uses nested structs and STL containers (map, vector)
 * Python uses nested dictionaries
 */
struct ExpenseSummary {
    double total;
    int count;
    std::map<std::string, double> by_category;      // STL map vs Python dict
    std::map<std::string, double> by_user;          // STL map vs Python dict
    std::map<std::string, std::vector<Expense>> user_expenses;  // STL containers
    
    // C++ FEATURE: Constructor with member initialization
    ExpenseSummary() : total(0.0), count(0) {}
};

#endif // EXPENSE_H

