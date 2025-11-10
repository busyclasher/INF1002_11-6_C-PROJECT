# Student Records CMS - Test Suite Documentation

This directory contains the test suite for the Student Records CMS project using the Unity test framework.

## Directory Structure

```
tests/
├── unity/                    # Unity test framework
│   ├── unity.h              # Unity header file
│   └── unity.c              # Unity implementation
├── test_data/               # Test database files
│   ├── test_valid.txt       # Valid test data (10 records)
│   ├── test_invalid.txt     # Invalid/malformed test data
│   ├── test_empty.txt       # Empty database template
│   └── test_single.txt      # Single record test file
├── test_utils.c             # Utility functions tests
├── test_database.c          # Database operations tests
├── test_summary.c           # Sorting and statistics tests
├── test_commands.c          # Command processing tests
├── test_runner.c            # Main test runner (optional)
├── build_tests.bat          # Windows build script
├── run_tests.bat            # Windows test runner script
├── Makefile                 # Linux/macOS build file
└── README_TESTS.md          # This file
```

## Building and Running Tests

### Windows (using batch files)

#### Build all tests:
```bash
cd tests
build_tests.bat
```

#### Build and run all tests:
```bash
cd tests
run_tests.bat
```

#### Run individual test suites:
```bash
build\test_utils.exe
build\test_database.exe
build\test_summary.exe
build\test_commands.exe
```

### Linux/macOS (using Makefile)

#### Build all tests:
```bash
cd tests
make
```

#### Build and run all tests:
```bash
cd tests
make test
```

#### Run individual test suites:
```bash
./build/test_utils
./build/test_database
./build/test_summary
./build/test_commands
```

#### Clean build files:
```bash
make clean
```

## Test Coverage

### test_utils.c
Tests for utility functions in `utils.c`:
- Student ID validation (valid/invalid IDs)
- Name validation (NULL, empty, too long)
- Programme validation (NULL, empty, too long)
- Mark validation (negative, too high, boundary values)
- String operations (uppercase, trimming)

### test_database.c
Tests for database operations in `database.c`:
- Database initialization and cleanup
- Loading from file (valid, empty, invalid, nonexistent)
- Saving to file
- Record insertion (valid, duplicate ID, NULL arguments)
- Record querying (existing, nonexistent, NULL arguments)
- Record updating (valid, NULL arguments)
- Record deletion (existing, nonexistent, NULL arguments)

### test_summary.c
Tests for sorting and statistics in `summary.c`:
- Sorting by ID (ascending/descending, NULL/empty database)
- Sorting by mark (ascending/descending, NULL/empty database)
- Summary calculation (valid data, NULL arguments, empty database, single record)
- Summary display (valid data, NULL/empty database)

### test_commands.c
Tests for command processing in `commands.c`:
- OPEN command (valid file, empty file, nonexistent file, NULL arguments)
- SHOW command (ALL, SUMMARY, ID/MARK with ASC/DESC, NULL database)
- INSERT command (valid data, NULL database)
- QUERY command (existing/nonexistent record, NULL database)
- UPDATE command (valid data, NULL database)
- DELETE command (existing/nonexistent record, NULL database)
- SAVE command (default/custom path, NULL database)
- HELP command
- Command parsing (valid/invalid commands, NULL arguments)

## Understanding Test Results

### Success Output
```
.....
-----------------------
15 Tests 0 Failures 0 Ignored
OK
```

### Failure Output
```
....test_function_name:42:FAIL: Expected 5 Was 3
..
-----------------------
15 Tests 1 Failures 0 Ignored
FAIL
```

### Ignored Tests
Some tests are marked as `TEST_IGNORE()` because they:
- Require interactive input simulation
- Depend on features not yet implemented
- Are integration tests requiring full system setup

## Writing New Tests

### Test File Template

```c
#include "unity/unity.h"
#include "../include/your_module.h"

void setUp(void) {
    // Runs before each test
}

void tearDown(void) {
    // Runs after each test
}

void test_your_function_name(void) {
    // Arrange
    int expected = 5;
    
    // Act
    int actual = your_function();
    
    // Assert
    TEST_ASSERT_EQUAL(expected, actual);
}

int main(void) {
    UnityBegin("test_your_module.c");
    
    RUN_TEST(test_your_function_name);
    
    return UnityEnd();
}
```

### Common Unity Assertions

```c
TEST_ASSERT(condition)                          // Assert condition is true
TEST_ASSERT_TRUE(condition)                     // Assert true
TEST_ASSERT_FALSE(condition)                    // Assert false
TEST_ASSERT_EQUAL(expected, actual)             // Assert equality
TEST_ASSERT_EQUAL_INT(expected, actual)         // Assert int equality
TEST_ASSERT_EQUAL_FLOAT(expected, actual)       // Assert float equality
TEST_ASSERT_EQUAL_STRING(expected, actual)      // Assert string equality
TEST_ASSERT_NULL(pointer)                       // Assert pointer is NULL
TEST_ASSERT_NOT_NULL(pointer)                   // Assert pointer is not NULL
TEST_FAIL_MESSAGE(message)                      // Force failure with message
TEST_IGNORE_MESSAGE(message)                    // Ignore test with message
```

## Adding Tests to Build System

### Windows (build_tests.bat)
Add a new compilation command:
```batch
echo [N/N] Compiling test_newmodule...
%CC% %CFLAGS% -o build/test_newmodule.exe test_newmodule.c unity/unity.c %SRC_FILES%
```

### Linux/macOS (Makefile)
1. Add test executable variable:
```makefile
TEST_NEWMODULE = $(BUILD_DIR)/test_newmodule
```

2. Add to `all` target:
```makefile
all: build_dir ... $(TEST_NEWMODULE)
```

3. Add build rule:
```makefile
$(TEST_NEWMODULE): test_newmodule.c $(UNITY_SRC) $(SRC_FILES)
	@echo "Compiling test_newmodule..."
	$(CC) $(CFLAGS) -o $@ $^
```

## Test Data Files

### test_valid.txt
Contains 10 valid student records with varied marks (56.7 to 95.2) for comprehensive testing of sorting, statistics, and CRUD operations.

### test_invalid.txt
Contains intentionally malformed records:
- Invalid student ID (non-numeric)
- Missing programme field
- Mark exceeding 100.0
- Missing student ID
- Negative mark

### test_empty.txt
Contains only headers with no records, used to test empty database handling.

### test_single.txt
Contains exactly one record, useful for boundary testing.

## Continuous Integration

To integrate with CI/CD:

### GitHub Actions (example)
```yaml
- name: Run Tests
  run: |
    cd tests
    make test
```

### Manual Testing Workflow
1. Make code changes
2. Build tests: `make` or `build_tests.bat`
3. Run tests: `make test` or `run_tests.bat`
4. Fix any failures
5. Commit changes

## Troubleshooting

### Tests won't compile
- Ensure GCC is installed and in PATH
- Check that all source files exist in `../src/`
- Verify include paths are correct

### Tests crash or hang
- Check for NULL pointer dereferences
- Ensure setUp() properly initializes test data
- Verify tearDown() cleans up resources

### Tests fail unexpectedly
- Check if implementation is complete (many tests are commented out until features are implemented)
- Verify test data files exist in `test_data/`
- Review expected vs actual values in failure messages

## Best Practices

1. **Run tests frequently** - After each significant code change
2. **Write tests first** - Consider TDD (Test-Driven Development)
3. **Keep tests isolated** - Each test should be independent
4. **Use descriptive names** - Test names should describe what they test
5. **Test edge cases** - NULL pointers, empty data, boundary values
6. **Clean up resources** - Use tearDown() to prevent memory leaks
7. **Comment wisely** - Explain complex test scenarios

## Contributing

When adding new features:
1. Write tests for the new functionality
2. Ensure all existing tests still pass
3. Update this documentation if needed
4. Run full test suite before committing

---

For questions or issues with the test suite, please contact the development team.

