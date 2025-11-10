# Makefile for Student Records CMS Test Suite
# For Linux/macOS or Windows with MinGW

CC = gcc
CFLAGS = -I../include -I./unity -Wall -std=c11
SRC_DIR = ../src
UNITY_DIR = ./unity
BUILD_DIR = ./build

# Source files
SRC_FILES = $(SRC_DIR)/cms_status.c $(SRC_DIR)/database.c $(SRC_DIR)/summary.c $(SRC_DIR)/utils.c
UNITY_SRC = $(UNITY_DIR)/unity.c

# Test executables
TEST_UTILS = $(BUILD_DIR)/test_utils
TEST_DATABASE = $(BUILD_DIR)/test_database
TEST_SUMMARY = $(BUILD_DIR)/test_summary
TEST_COMMANDS = $(BUILD_DIR)/test_commands

# Targets
.PHONY: all clean test build_dir

all: build_dir $(TEST_UTILS) $(TEST_DATABASE) $(TEST_SUMMARY) $(TEST_COMMANDS)

build_dir:
	@mkdir -p $(BUILD_DIR)

$(TEST_UTILS): test_utils.c $(UNITY_SRC) $(SRC_FILES)
	@echo "Compiling test_utils..."
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_DATABASE): test_database.c $(UNITY_SRC) $(SRC_FILES)
	@echo "Compiling test_database..."
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_SUMMARY): test_summary.c $(UNITY_SRC) $(SRC_FILES)
	@echo "Compiling test_summary..."
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_COMMANDS): test_commands.c $(UNITY_SRC) $(SRC_FILES) $(SRC_DIR)/commands.c
	@echo "Compiling test_commands..."
	$(CC) $(CFLAGS) -o $@ $^

test: all
	@echo "================================="
	@echo "Running Test Suites"
	@echo "================================="
	@echo ""
	@echo "[1/4] Running test_utils..."
	@$(TEST_UTILS)
	@echo ""
	@echo "[2/4] Running test_database..."
	@$(TEST_DATABASE)
	@echo ""
	@echo "[3/4] Running test_summary..."
	@$(TEST_SUMMARY)
	@echo ""
	@echo "[4/4] Running test_commands..."
	@$(TEST_COMMANDS)
	@echo ""
	@echo "================================="
	@echo "All Tests Completed"
	@echo "================================="

clean:
	@echo "Cleaning test build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

help:
	@echo "Student Records CMS Test Suite"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build all tests"
	@echo "  make test     - Build and run all tests"
	@echo "  make clean    - Remove build files"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Individual test executables:"
	@echo "  $(TEST_UTILS)"
	@echo "  $(TEST_DATABASE)"
	@echo "  $(TEST_SUMMARY)"
	@echo "  $(TEST_COMMANDS)"

