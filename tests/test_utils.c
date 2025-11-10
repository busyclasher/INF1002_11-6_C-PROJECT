#include "unity/unity.h"
#include "../include/utils.h"
#include "../include/config.h"
#include <string.h>

/* Test setUp - runs before each test */
void setUp(void) {
    /* TODO: Add any setup code needed before each test */
}

/* Test tearDown - runs after each test */
void tearDown(void) {
    /* TODO: Add any cleanup code needed after each test */
}

/* ===== Student ID Validation Tests ===== */

void test_validate_student_id_valid(void) {
    /* TODO: Test valid student IDs */
    TEST_ASSERT_TRUE(cms_validate_student_id(2301234));
    TEST_ASSERT_TRUE(cms_validate_student_id(1000000));
    TEST_ASSERT_TRUE(cms_validate_student_id(9999999));
}

void test_validate_student_id_invalid(void) {
    /* TODO: Test invalid student IDs */
    TEST_ASSERT_FALSE(cms_validate_student_id(0));
    TEST_ASSERT_FALSE(cms_validate_student_id(-1));
    TEST_ASSERT_FALSE(cms_validate_student_id(-12345));
}

/* ===== Name Validation Tests ===== */

void test_validate_name_valid(void) {
    /* TODO: Test valid names */
    TEST_ASSERT_TRUE(cms_validate_name("Joshua Chen"));
    TEST_ASSERT_TRUE(cms_validate_name("A"));
    TEST_ASSERT_TRUE(cms_validate_name("Muhammad bin Abdullah"));
}

void test_validate_name_invalid_null(void) {
    /* TODO: Test NULL name */
    TEST_ASSERT_FALSE(cms_validate_name(NULL));
}

void test_validate_name_invalid_empty(void) {
    /* TODO: Test empty name */
    TEST_ASSERT_FALSE(cms_validate_name(""));
}

void test_validate_name_invalid_too_long(void) {
    /* TODO: Test name exceeding maximum length */
    char long_name[CMS_MAX_NAME_LEN + 10];
    memset(long_name, 'A', CMS_MAX_NAME_LEN + 5);
    long_name[CMS_MAX_NAME_LEN + 5] = '\0';
    
    TEST_ASSERT_FALSE(cms_validate_name(long_name));
}

/* ===== Programme Validation Tests ===== */

void test_validate_programme_valid(void) {
    /* TODO: Test valid programme names */
    TEST_ASSERT_TRUE(cms_validate_programme("Computer Science"));
    TEST_ASSERT_TRUE(cms_validate_programme("Software Engineering"));
    TEST_ASSERT_TRUE(cms_validate_programme("Cybersecurity"));
}

void test_validate_programme_invalid_null(void) {
    /* TODO: Test NULL programme */
    TEST_ASSERT_FALSE(cms_validate_programme(NULL));
}

void test_validate_programme_invalid_empty(void) {
    /* TODO: Test empty programme */
    TEST_ASSERT_FALSE(cms_validate_programme(""));
}

void test_validate_programme_invalid_too_long(void) {
    /* TODO: Test programme exceeding maximum length */
    char long_programme[CMS_MAX_PROGRAMME_LEN + 10];
    memset(long_programme, 'A', CMS_MAX_PROGRAMME_LEN + 5);
    long_programme[CMS_MAX_PROGRAMME_LEN + 5] = '\0';
    
    TEST_ASSERT_FALSE(cms_validate_programme(long_programme));
}

/* ===== Mark Validation Tests ===== */

void test_validate_mark_valid(void) {
    /* TODO: Test valid marks */
    TEST_ASSERT_TRUE(cms_validate_mark(0.0f));
    TEST_ASSERT_TRUE(cms_validate_mark(50.0f));
    TEST_ASSERT_TRUE(cms_validate_mark(100.0f));
    TEST_ASSERT_TRUE(cms_validate_mark(75.5f));
}

void test_validate_mark_invalid_negative(void) {
    /* TODO: Test negative marks */
    TEST_ASSERT_FALSE(cms_validate_mark(-0.1f));
    TEST_ASSERT_FALSE(cms_validate_mark(-50.0f));
}

void test_validate_mark_invalid_too_high(void) {
    /* TODO: Test marks above maximum */
    TEST_ASSERT_FALSE(cms_validate_mark(100.1f));
    TEST_ASSERT_FALSE(cms_validate_mark(150.0f));
}

void test_validate_mark_boundary(void) {
    /* TODO: Test boundary values */
    TEST_ASSERT_TRUE(cms_validate_mark(CMS_MIN_MARK));
    TEST_ASSERT_TRUE(cms_validate_mark(CMS_MAX_MARK));
}

/* ===== String Utility Tests ===== */

void test_string_to_upper(void) {
    /* TODO: Test string uppercase conversion */
    char test_str[50];
    
    strcpy(test_str, "hello world");
    cms_string_to_upper(test_str);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", test_str);
    
    strcpy(test_str, "Test123");
    cms_string_to_upper(test_str);
    TEST_ASSERT_EQUAL_STRING("TEST123", test_str);
}

void test_string_to_upper_null(void) {
    /* TODO: Test NULL string handling */
    cms_string_to_upper(NULL);  /* Should not crash */
}

void test_trim_string(void) {
    /* TODO: Test string trimming */
    char test_str[50];
    
    strcpy(test_str, "  hello  ");
    cms_trim_string(test_str);
    /* Note: Implementation needed in utils.c */
    /* TEST_ASSERT_EQUAL_STRING("hello", test_str); */
}

/* Main test runner for this module */
int main(void) {
    UnityBegin("test_utils.c");
    
    /* Run student ID validation tests */
    RUN_TEST(test_validate_student_id_valid);
    RUN_TEST(test_validate_student_id_invalid);
    
    /* Run name validation tests */
    RUN_TEST(test_validate_name_valid);
    RUN_TEST(test_validate_name_invalid_null);
    RUN_TEST(test_validate_name_invalid_empty);
    RUN_TEST(test_validate_name_invalid_too_long);
    
    /* Run programme validation tests */
    RUN_TEST(test_validate_programme_valid);
    RUN_TEST(test_validate_programme_invalid_null);
    RUN_TEST(test_validate_programme_invalid_empty);
    RUN_TEST(test_validate_programme_invalid_too_long);
    
    /* Run mark validation tests */
    RUN_TEST(test_validate_mark_valid);
    RUN_TEST(test_validate_mark_invalid_negative);
    RUN_TEST(test_validate_mark_invalid_too_high);
    RUN_TEST(test_validate_mark_boundary);
    
    /* Run string utility tests */
    RUN_TEST(test_string_to_upper);
    RUN_TEST(test_string_to_upper_null);
    RUN_TEST(test_trim_string);
    
    return UnityEnd();
}

