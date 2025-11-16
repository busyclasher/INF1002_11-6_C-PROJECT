#include "unity/unity.h"
#include "../include/summary.h"
#include "../include/database.h"
#include "../include/cms.h"
#include <string.h>

/* Global test database */
static StudentDatabase test_db;

/* Helper function to add test records */
void add_test_records(void)
{
    /* TODO: Helper to populate database with test records */
    /* This will be useful once database_insert is implemented */

    StudentRecord records[] = {
        {2301234, "Alice", "Computer Science", 85.5f},
        {2201234, "Bob", "Software Engineering", 72.0f},
        {2304567, "Charlie", "Cybersecurity", 90.5f},
        {2202345, "David", "Information Systems", 65.0f},
        {2305678, "Eve", "Digital Supply Chain", 78.5f}};

    /* Note: Uncomment when insert is implemented */
    /* for (int i = 0; i < 5; i++) {
        cms_database_insert(&test_db, &records[i]);
    } */
}

/* Test setUp - runs before each test */
void setUp(void)
{
    /* TODO: Initialize database and add test records */
    cms_database_init(&test_db);
}

/* Test tearDown - runs after each test */
void tearDown(void)
{
    /* TODO: Cleanup database after each test */
    cms_database_cleanup(&test_db);
}

/* ===== Sort by ID Tests ===== */

void test_sort_by_id_ascending(void)
{
    /* TODO: Test sorting by ID in ascending order */
    add_test_records();

    CMS_STATUS status = cms_sort_by_id(&test_db, SORT_ASCENDING);

    /* Note: Will work once records are added */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_TRUE(test_db.records[0].id < test_db.records[1].id); */
}

void test_sort_by_id_descending(void)
{
    /* TODO: Test sorting by ID in descending order */
    add_test_records();

    CMS_STATUS status = cms_sort_by_id(&test_db, SORT_DESCENDING);

    /* Note: Will work once records are added */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_TRUE(test_db.records[0].id > test_db.records[1].id); */
}

void test_sort_by_id_null_database(void)
{
    /* TODO: Test sort with NULL database */
    CMS_STATUS status = cms_sort_by_id(NULL, SORT_ASCENDING);
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

void test_sort_by_id_empty_database(void)
{
    /* TODO: Test sort with empty database */
    CMS_STATUS status = cms_sort_by_id(&test_db, SORT_ASCENDING);

    /* Should handle empty database gracefully */
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

/* ===== Sort by Mark Tests ===== */

void test_sort_by_mark_ascending(void)
{
    /* TODO: Test sorting by mark in ascending order */
    add_test_records();

    CMS_STATUS status = cms_sort_by_mark(&test_db, SORT_ASCENDING);

    /* Note: Will work once records are added */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_TRUE(test_db.records[0].mark <= test_db.records[1].mark); */
}

void test_sort_by_mark_descending(void)
{
    /* TODO: Test sorting by mark in descending order */
    add_test_records();

    CMS_STATUS status = cms_sort_by_mark(&test_db, SORT_DESCENDING);

    /* Note: Will work once records are added */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_TRUE(test_db.records[0].mark >= test_db.records[1].mark); */
}

void test_sort_by_mark_null_database(void)
{
    /* TODO: Test sort with NULL database */
    CMS_STATUS status = cms_sort_by_mark(NULL, SORT_ASCENDING);
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

void test_sort_by_mark_empty_database(void)
{
    /* TODO: Test sort with empty database */
    CMS_STATUS status = cms_sort_by_mark(&test_db, SORT_ASCENDING);

    /* Should handle empty database gracefully */
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

/* ===== Summary Statistics Tests ===== */

void test_calculate_summary_valid(void)
{
    /* TODO: Test summary calculation with valid data */
    add_test_records();

    SummaryStats stats;
    CMS_STATUS status = cms_calculate_summary(&test_db, &stats);

    /* Note: Will work once records are added and function is implemented */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_EQUAL(5, stats.count); */
    /* TEST_ASSERT_EQUAL_FLOAT(78.3f, stats.average); */
    /* TEST_ASSERT_EQUAL_FLOAT(90.5f, stats.highest); */
    /* TEST_ASSERT_EQUAL_FLOAT(65.0f, stats.lowest); */
}

void test_calculate_summary_null_arguments(void)
{
    /* TODO: Test summary with NULL arguments */
    SummaryStats stats;

    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, cms_calculate_summary(NULL, &stats));
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, cms_calculate_summary(&test_db, NULL));
}

void test_calculate_summary_empty_database(void)
{
    /* TODO: Test summary with empty database */
    SummaryStats stats;
    CMS_STATUS status = cms_calculate_summary(&test_db, &stats);

    /* Should return error for empty database */
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

void test_calculate_summary_single_record(void)
{
    /* TODO: Test summary with single record */
    StudentRecord record = {2301234, "Single", "CS", 75.0f};

    /* Note: Uncomment when insert is implemented */
    /* cms_database_insert(&test_db, &record); */

    SummaryStats stats;
    /* CMS_STATUS status = cms_calculate_summary(&test_db, &stats); */

    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
    /* TEST_ASSERT_EQUAL(1, stats.count); */
    /* TEST_ASSERT_EQUAL_FLOAT(75.0f, stats.average); */
    /* TEST_ASSERT_EQUAL_FLOAT(75.0f, stats.highest); */
    /* TEST_ASSERT_EQUAL_FLOAT(75.0f, stats.lowest); */
}

/* ===== Display Summary Tests ===== */

void test_display_summary_valid(void)
{
    /* TODO: Test summary display with valid data */
    add_test_records();

    CMS_STATUS status = cms_display_summary(&test_db);

    /* Note: Will work once records are added and function is implemented */
    /* TEST_ASSERT_EQUAL(CMS_STATUS_OK, status); */
}

void test_display_summary_null_database(void)
{
    /* TODO: Test display with NULL database */
    CMS_STATUS status = cms_display_summary(NULL);
    TEST_ASSERT_EQUAL(CMS_STATUS_INVALID_ARGUMENT, status);
}

void test_display_summary_empty_database(void)
{
    /* TODO: Test display with empty database */
    CMS_STATUS status = cms_display_summary(&test_db);

    /* Should handle empty database gracefully */
    /* TEST_ASSERT_NOT_EQUAL(CMS_STATUS_OK, status); */
}

/* Main test runner for this module */
int main(void)
{
    UnityBegin("test_summary.c");

    /* Sort by ID tests */
    RUN_TEST(test_sort_by_id_ascending);
    RUN_TEST(test_sort_by_id_descending);
    RUN_TEST(test_sort_by_id_null_database);
    RUN_TEST(test_sort_by_id_empty_database);

    /* Sort by mark tests */
    RUN_TEST(test_sort_by_mark_ascending);
    RUN_TEST(test_sort_by_mark_descending);
    RUN_TEST(test_sort_by_mark_null_database);
    RUN_TEST(test_sort_by_mark_empty_database);

    /* Summary calculation tests */
    RUN_TEST(test_calculate_summary_valid);
    RUN_TEST(test_calculate_summary_null_arguments);
    RUN_TEST(test_calculate_summary_empty_database);
    RUN_TEST(test_calculate_summary_single_record);

    /* Display summary tests */
    RUN_TEST(test_display_summary_valid);
    RUN_TEST(test_display_summary_null_database);
    RUN_TEST(test_display_summary_empty_database);

    return UnityEnd();
}
