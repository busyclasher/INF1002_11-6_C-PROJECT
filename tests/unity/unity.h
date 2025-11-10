#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#include <setjmp.h>
#include <stddef.h>

/* Unity Test Framework - Simplified Version */

/* Test result tracking */
typedef struct {
    int tests_run;
    int tests_failed;
    int tests_ignored;
    const char* current_test_name;
    jmp_buf abort_frame;
    int within_test;
} Unity;

extern Unity Unity_instance;

/* Core Unity Functions */
void UnityBegin(const char* filename);
int UnityEnd(void);
void UnityDefaultTestRun(void (*func)(void), const char* name, int line);

/* Setup and Teardown */
void setUp(void);
void tearDown(void);

/* Test Assertions */
void UnityAssertEqualNumber(const int expected, const int actual, const char* msg, const int line);
void UnityAssertEqualFloat(const float expected, const float actual, const float delta, const char* msg, const int line);
void UnityAssertEqualString(const char* expected, const char* actual, const char* msg, const int line);
void UnityAssertNull(const void* pointer, const char* msg, const int line);
void UnityAssertNotNull(const void* pointer, const char* msg, const int line);
void UnityAssertTrue(const int condition, const char* msg, const int line);
void UnityAssertFalse(const int condition, const char* msg, const int line);
void UnityFail(const char* msg, const int line);
void UnityIgnore(const char* msg, const int line);

/* Macro Helpers */
#define RUN_TEST(func) UnityDefaultTestRun(func, #func, __LINE__)

#define TEST_ASSERT(condition) \
    UnityAssertTrue((condition), "Expected TRUE", __LINE__)

#define TEST_ASSERT_TRUE(condition) \
    UnityAssertTrue((condition), "Expected TRUE", __LINE__)

#define TEST_ASSERT_FALSE(condition) \
    UnityAssertFalse((condition), "Expected FALSE", __LINE__)

#define TEST_ASSERT_EQUAL(expected, actual) \
    UnityAssertEqualNumber((expected), (actual), NULL, __LINE__)

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    UnityAssertEqualNumber((int)(expected), (int)(actual), NULL, __LINE__)

#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) \
    UnityAssertEqualFloat((expected), (actual), 0.001f, NULL, __LINE__)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    UnityAssertEqualString((expected), (actual), NULL, __LINE__)

#define TEST_ASSERT_NULL(pointer) \
    UnityAssertNull((pointer), "Expected NULL", __LINE__)

#define TEST_ASSERT_NOT_NULL(pointer) \
    UnityAssertNotNull((pointer), "Expected NOT NULL", __LINE__)

#define TEST_FAIL_MESSAGE(message) \
    UnityFail((message), __LINE__)

#define TEST_FAIL() \
    UnityFail("Explicit Fail", __LINE__)

#define TEST_IGNORE_MESSAGE(message) \
    UnityIgnore((message), __LINE__)

#define TEST_IGNORE() \
    UnityIgnore("Test Ignored", __LINE__)

#define TEST_MESSAGE(message) \
    UnityPrint(message)

/* Print helpers */
void UnityPrint(const char* string);
void UnityPrintNumber(const int number);

#endif /* UNITY_FRAMEWORK_H */

