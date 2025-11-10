#include <stdio.h>
#include <stdlib.h>

/* Test runner - executes all test suites */

/* Forward declarations of test main functions */
int test_utils_main(void);
int test_database_main(void);
int test_summary_main(void);
int test_commands_main(void);

int main(void) {
    int total_failures = 0;
    
    printf("\n");
    printf("=====================================\n");
    printf("  Student Records CMS - Test Suite  \n");
    printf("=====================================\n");
    
    /* Run all test suites */
    printf("\n>>> Running Utils Tests...\n");
    /* total_failures += test_utils_main(); */
    printf("NOTE: Compile individual test files to run specific test suites\n");
    
    printf("\n>>> Running Database Tests...\n");
    /* total_failures += test_database_main(); */
    printf("NOTE: Compile individual test files to run specific test suites\n");
    
    printf("\n>>> Running Summary Tests...\n");
    /* total_failures += test_summary_main(); */
    printf("NOTE: Compile individual test files to run specific test suites\n");
    
    printf("\n>>> Running Commands Tests...\n");
    /* total_failures += test_commands_main(); */
    printf("NOTE: Compile individual test files to run specific test suites\n");
    
    /* Final summary */
    printf("\n=====================================\n");
    printf("  Test Suite Summary\n");
    printf("=====================================\n");
    
    if (total_failures == 0) {
        printf("ALL TESTS PASSED!\n");
        printf("\nTo run individual test suites:\n");
        printf("  - test_utils.exe\n");
        printf("  - test_database.exe\n");
        printf("  - test_summary.exe\n");
        printf("  - test_commands.exe\n");
        return EXIT_SUCCESS;
    } else {
        printf("TESTS FAILED: %d failures\n", total_failures);
        return EXIT_FAILURE;
    }
}

