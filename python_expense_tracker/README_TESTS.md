# Running Tests for Python Expense Tracker

## Quick Start

### 1. Set up Virtual Environment

```bash
# Create virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate  # On macOS/Linux
# or
venv\Scripts\activate  # On Windows
```

### 2. Install Dependencies

```bash
pip install -r requirements.txt
```

### 3. Run Tests

```bash
# Run all tests with coverage
pytest tests/ -v --cov=. --cov-report=html --cov-report=term

# Run all tests (simple)
pytest tests/

# Run specific test file
pytest tests/test_models.py -v

# Run specific test
pytest tests/test_models.py::test_create_user_success -v
```

### 4. View Coverage Report

```bash
# Open HTML coverage report in browser
open htmlcov/index.html  # macOS
xdg-open htmlcov/index.html  # Linux
start htmlcov/index.html  # Windows
```

## Test Structure

```
python_expense_tracker/
├── tests/
│   ├── __init__.py                    # Test package
│   ├── conftest.py                    # Pytest fixtures
│   ├── test_database.py               # Database tests (15 tests)
│   ├── test_models.py                 # CRUD tests (37 tests)
│   ├── test_utils.py                  # Validation tests (28 tests)
│   ├── test_expense_operations.py     # Business logic tests (25 tests)
│   ├── test_integration.py            # Integration tests (10 tests)
│   └── test_functional.py             # Functional tests (10 tests)
├── pytest.ini                         # Pytest configuration
└── requirements.txt                   # Dependencies
```

## Test Commands

### Basic Commands

```bash
# Run all tests
pytest

# Verbose output
pytest -v

# Show print statements
pytest -s

# Stop on first failure
pytest -x

# Run last failed tests
pytest --lf
```

### Coverage Commands

```bash
# Run with coverage
pytest --cov=.

# Coverage with HTML report
pytest --cov=. --cov-report=html

# Coverage with terminal report
pytest --cov=. --cov-report=term-missing

# Coverage for specific module
pytest --cov=models tests/test_models.py
```

### Filtering Tests

```bash
# Run tests by keyword
pytest -k "user"  # Runs all tests with "user" in name

# Run tests by marker
pytest -m "slow"  # If you add markers

# Run specific file
pytest tests/test_database.py

# Run specific test
pytest tests/test_database.py::test_init_database_creates_tables
```

### Output Options

```bash
# Quiet mode (less output)
pytest -q

# Show local variables on failure
pytest -l

# Show traceback styles
pytest --tb=short  # Short traceback
pytest --tb=line   # One line per failure
pytest --tb=no     # No traceback
```

## Test Results

Expected output:

```
============================= 120 passed in 24.84s =============================

================================ tests coverage ================================
Name                               Stmts   Miss  Cover
----------------------------------------------------------------
database.py                           32     19    41%
expense_operations.py                190     43    77%
models.py                            116      1    99%
utils.py                             158     28    82%
----------------------------------------------------------------
TOTAL                               1611    314    81%
Required test coverage of 80% reached. Total coverage: 80.51%
```

## Troubleshooting

### Issue: ModuleNotFoundError

**Solution**: Make sure you're in the correct directory and virtual environment is activated

```bash
cd python_expense_tracker
source venv/bin/activate
```

### Issue: pytest not found

**Solution**: Install pytest

```bash
pip install pytest pytest-cov pytest-mock freezegun
```

### Issue: Import errors

**Solution**: Make sure all modules are in the same directory as tests

```bash
ls  # Should show: database.py, models.py, utils.py, expense_operations.py, tests/
```

### Issue: Database errors

**Solution**: Tests use temporary databases, but if you see persistent errors:

```bash
# Remove any test database files
rm -f test_*.db
rm -f /tmp/tmp*.db
```

## Understanding Test Output

### Test Status Symbols

- `.` = Passed
- `F` = Failed
- `E` = Error
- `s` = Skipped
- `x` = Expected failure
- `X` = Unexpected pass

### Coverage Metrics

- **Stmts**: Total statements in file
- **Miss**: Statements not covered by tests
- **Cover**: Percentage covered

### Example Test Output

```
tests/test_models.py::test_create_user_success PASSED                    [ 15%]
tests/test_models.py::test_create_user_duplicate_name_raises_error PASSED [ 16%]
```

## Best Practices

1. **Run tests before committing code**

   ```bash
   pytest tests/ -v
   ```

2. **Check coverage regularly**

   ```bash
   pytest --cov=. --cov-report=term-missing
   ```

3. **Fix failing tests immediately**

   ```bash
   pytest --lf  # Run last failed
   ```

4. **Write tests for new features**

   - Add tests in appropriate test file
   - Follow existing test patterns
   - Aim for 80%+ coverage

5. **Keep tests fast**
   - Use fixtures for common setup
   - Avoid unnecessary database operations
   - Mock external dependencies

## Continuous Integration

For CI/CD pipelines:

```bash
# Install dependencies
pip install -r requirements.txt

# Run tests with coverage
pytest tests/ --cov=. --cov-report=xml --cov-fail-under=80

# Generate coverage badge
# (requires coverage-badge package)
coverage-badge -o coverage.svg
```

## Additional Resources

- [Pytest Documentation](https://docs.pytest.org/)
- [Coverage.py Documentation](https://coverage.readthedocs.io/)
- [Pytest-cov Plugin](https://pytest-cov.readthedocs.io/)

## Support

For issues or questions:

1. Check TEST_SUMMARY.md for detailed test information
2. Review test files for examples
3. Check pytest documentation
4. Ensure all dependencies are installed
