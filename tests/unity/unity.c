#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Global Unity instance */
Unity Unity_instance;

/* Initialize Unity for a test run */
void UnityBegin(const char* filename) {
    Unity_instance.tests_run = 0;
    Unity_instance.tests_failed = 0;
    Unity_instance.tests_ignored = 0;
    Unity_instance.within_test = 0;
    printf("\n----- Testing %s -----\n", filename);
}

/* End Unity test run and report results */
int UnityEnd(void) {
    printf("\n-----------------------\n");
    printf("%d Tests %d Failures %d Ignored\n", 
           Unity_instance.tests_run, 
           Unity_instance.tests_failed,
           Unity_instance.tests_ignored);
    
    if (Unity_instance.tests_failed == 0) {
        printf("OK\n");
    } else {
        printf("FAIL\n");
    }
    
    return Unity_instance.tests_failed;
}

/* Run a single test */
void UnityDefaultTestRun(void (*func)(void), const char* name, int line) {
    Unity_instance.current_test_name = name;
    Unity_instance.within_test = 1;
    Unity_instance.tests_run++;
    
    if (setjmp(Unity_instance.abort_frame) == 0) {
        setUp();
        func();
        tearDown();
        printf(".");
    }
    
    Unity_instance.within_test = 0;
}

/* Print helpers */
void UnityPrint(const char* string) {
    if (string != NULL) {
        printf("%s", string);
    }
}

void UnityPrintNumber(const int number) {
    printf("%d", number);
}

/* Assertion implementations */
void UnityAssertEqualNumber(const int expected, const int actual, const char* msg, const int line) {
    if (expected != actual) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected %d Was %d", 
               Unity_instance.current_test_name, line, expected, actual);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertEqualFloat(const float expected, const float actual, const float delta, const char* msg, const int line) {
    float diff = expected - actual;
    if (diff < 0) diff = -diff;
    
    if (diff > delta) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected %.2f Was %.2f", 
               Unity_instance.current_test_name, line, expected, actual);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertEqualString(const char* expected, const char* actual, const char* msg, const int line) {
    if (expected == NULL && actual == NULL) {
        return;
    }
    
    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected '%s' Was '%s'", 
               Unity_instance.current_test_name, line, 
               expected ? expected : "NULL", 
               actual ? actual : "NULL");
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertNull(const void* pointer, const char* msg, const int line) {
    if (pointer != NULL) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected NULL Was 0x%p", 
               Unity_instance.current_test_name, line, pointer);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertNotNull(const void* pointer, const char* msg, const int line) {
    if (pointer == NULL) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected NOT NULL", 
               Unity_instance.current_test_name, line);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertTrue(const int condition, const char* msg, const int line) {
    if (!condition) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected TRUE Was FALSE", 
               Unity_instance.current_test_name, line);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityAssertFalse(const int condition, const char* msg, const int line) {
    if (condition) {
        Unity_instance.tests_failed++;
        printf("\n%s:%d:FAIL: Expected FALSE Was TRUE", 
               Unity_instance.current_test_name, line);
        if (msg) printf(" [%s]", msg);
        longjmp(Unity_instance.abort_frame, 1);
    }
}

void UnityFail(const char* msg, const int line) {
    Unity_instance.tests_failed++;
    printf("\n%s:%d:FAIL: %s", 
           Unity_instance.current_test_name, line, msg);
    longjmp(Unity_instance.abort_frame, 1);
}

void UnityIgnore(const char* msg, const int line) {
    Unity_instance.tests_ignored++;
    printf("\n%s:%d:IGNORE: %s", 
           Unity_instance.current_test_name, line, msg);
    longjmp(Unity_instance.abort_frame, 1);
}

/* Weak implementations of setUp and tearDown */
void __attribute__((weak)) setUp(void) {}
void __attribute__((weak)) tearDown(void) {}

