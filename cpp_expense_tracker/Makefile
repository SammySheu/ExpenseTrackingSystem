# Makefile for Expense Tracking System (C++)
#
# C++ BUILD SYSTEM DEMONSTRATION:
# Unlike Python (interpreted), C++ requires compilation and linking
# This Makefile manages the build process
#
# CONTRAST WITH PYTHON:
# Python: No compilation needed, just run with 'python script.py'
# C++: Must compile source to object files, then link into executable

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lsqlite3

# Target executable
TARGET = expense_tracker

# Source files
SOURCES = main.cpp \
          database.cpp \
          models.cpp \
          expense_operations.cpp \
          utils.cpp

# Object files (generated from sources)
OBJECTS = $(SOURCES:.cpp=.o)

# Header dependencies
HEADERS = expense.h \
          database.h \
          models.h \
          expense_operations.h \
          utils.h

# Default target: build the executable
# This is what runs when you type 'make'
all: $(TARGET)

# Link object files into final executable
# C++ FEATURE: Separate compilation and linking stages
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Run with ./$(TARGET)"

# Compile each .cpp file into .o (object file)
# C++ FEATURE: Incremental compilation - only recompile changed files
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
# Removes compiled files, forcing full rebuild next time
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET) expenses.db
	@echo "Clean complete!"

# Clean only object files (keep executable)
clean-obj:
	@echo "Cleaning object files..."
	rm -f $(OBJECTS)

# Clean database only
clean-db:
	@echo "Cleaning database..."
	rm -f expenses.db

# Rebuild from scratch
rebuild: clean all

# Run the program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Display help
help:
	@echo "Expense Tracker Makefile"
	@echo "Available targets:"
	@echo "  make          - Build the executable"
	@echo "  make all      - Same as 'make'"
	@echo "  make clean    - Remove all build artifacts and database"
	@echo "  make rebuild  - Clean and rebuild from scratch"
	@echo "  make run      - Build and run the program"
	@echo "  make help     - Display this help message"
	@echo ""
	@echo "Build system demonstrates C++ compilation process:"
	@echo "  1. Compile each .cpp file to .o (object file)"
	@echo "  2. Link all .o files into final executable"
	@echo "  Python doesn't need this - it's interpreted directly!"

# Check for required dependencies
check-deps:
	@echo "Checking for required dependencies..."
	@command -v $(CXX) >/dev/null 2>&1 || { echo "g++ not found!"; exit 1; }
	@echo "g++ found: $$($(CXX) --version | head -n 1)"
	@echo "SQLite3 development libraries required for compilation"
	@echo "  Ubuntu/Debian: sudo apt-get install libsqlite3-dev"
	@echo "  macOS: brew install sqlite3"
	@echo "  Fedora: sudo dnf install sqlite-devel"

.PHONY: all clean clean-obj clean-db rebuild run help check-deps

