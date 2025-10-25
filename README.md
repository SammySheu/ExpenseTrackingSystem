# Group Project: Expense Tracking System (Team 7)

### Authors

@SammySheu

@Kmukasa

## Project Overview

The following project is an expense tracking system written in both Python and C++. The application allows users to record, view, categorize, and filter expenses by category, amount range, date range, and user.

## Project Architecture

- **Database Layer**: SQLite database with three tables (Users, Categories, Expenses)
- **Data Access Layer**: Setter and getter functions for accessing the tables in our SQLite database.
- **Business Logic Layer**: Core expense management operations, including adding a record, filtering, and viewing an expense. Includes input validation and error handling.
- **Presentational UI Layer**: Interactive CLI menu system, that asks users for inputs to denote the commands they wish to perform.

## Data Schema

### Database Tables

#### Expenses

| Column      | Type    | Description               |
| ----------- | ------- | ------------------------- |
| id          | INTEGER | Primary Key               |
| date        | TEXT    | Date of expense           |
| category_id | INTEGER | Foreign Key to Categories |
| title       | TEXT    | Expense description       |
| amount      | REAL    | Cost of expense           |
| created_at  | TEXT    | Record creation timestamp |
| user_id     | INTEGER | Foreign Key to Users      |

#### Categories

| Column | Type    | Description   |
| ------ | ------- | ------------- |
| id     | INTEGER | Primary Key   |
| name   | TEXT    | Category name |

#### Users

| Column | Type    | Description |
| ------ | ------- | ----------- |
| id     | INTEGER | Primary Key |
| name   | TEXT    | User's name |

## Running the Expense Tracking System

### Python

```
cd python_expense_tracker

python3 expense_tracker.py
```

### C++

```
cd c++_expense_tracker

g++ expense_tracker.cpp -o expense_tracker

./expense_tracker
```
