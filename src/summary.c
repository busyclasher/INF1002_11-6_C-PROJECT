#include <stdio.h>
#include <stdlib.h>
#include "../include/summary.h"
#include <string.h>
#include "../include/database.h"

static CmsGradeBucket cms_grade_bucket_from_mark(float mark)
{
    if (mark >= 85.0f)
    {
        return CMS_GRADE_A_PLUS;
    }
    if (mark >= 75.0f)
    {
        return CMS_GRADE_A;
    }
    if (mark >= 70.0f)
    {
        return CMS_GRADE_B_PLUS;
    }
    if (mark >= 65.0f)
    {
        return CMS_GRADE_B;
    }
    if (mark >= 60.0f)
    {
        return CMS_GRADE_C_PLUS;
    }
    if (mark >= 55.0f)
    {
        return CMS_GRADE_C;
    }
    if (mark >= 50.0f)
    {
        return CMS_GRADE_D;
    }
    return CMS_GRADE_F;
}

static const char *cms_grade_labels[CMS_GRADE_BUCKET_COUNT] = {
    "A+", "A", "B+", "B", "C+", "C", "D", "F"};

/* Comparison functions for qsort */
static int compare_by_id_asc(const void *a, const void *b)
{
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;
    return rec_a->id - rec_b->id;
}

static int compare_by_id_desc(const void *a, const void *b)
{
    return compare_by_id_asc(b, a);
}

static int compare_by_mark_asc(const void *a, const void *b)
{
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;

    if (rec_a->mark < rec_b->mark)
        return -1;
    if (rec_a->mark > rec_b->mark)
        return 1;
    return 0;
}

static int compare_by_mark_desc(const void *a, const void *b)
{
    return compare_by_mark_asc(b, a);
}

static int compare_by_name_asc(const void *a, const void *b)
{
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;

    return strcmp(rec_a->name, rec_b->name);
}

static int compare_by_name_desc(const void *a, const void *b)
{
    return compare_by_name_asc(b, a);
}

static int compare_by_programme_asc(const void *a, const void *b)
{
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;

    return strcmp(rec_a->programme, rec_b->programme);
}

static int compare_by_programme_desc(const void *a, const void *b)
{
    return compare_by_programme_asc(b, a);
}

CMS_STATUS cms_sort_by_id(StudentDatabase *db, SortOrder order)
{
    if (db == NULL || db->records == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING)
    {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_asc);
    }
    else
    {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_sort_by_mark(StudentDatabase *db, SortOrder order)
{
    if (db == NULL || db->records == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING)
    {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_asc);
    }
    else
    {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_calculate_summary(const StudentDatabase *db, SummaryStats *stats)
{
    if (db == NULL || stats == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        return CMS_STATUS_NOT_FOUND;
    }

    memset(stats, 0, sizeof(SummaryStats));
    stats->count = db->count;
    float total = 0.0f;

    stats->highest = db->records[0].mark;
    stats->lowest = db->records[0].mark;
    stats->highest_id = db->records[0].id;
    stats->lowest_id = db->records[0].id;

    for (size_t i = 0; i < db->count; ++i)
    {
        const StudentRecord *record = &db->records[i];
        total += record->mark;

        if (record->mark > stats->highest)
        {
            stats->highest = record->mark;
            stats->highest_id = record->id;
        }

        if (record->mark < stats->lowest)
        {
            stats->lowest = record->mark;
            stats->lowest_id = record->id;
        }

        CmsGradeBucket bucket = cms_grade_bucket_from_mark(record->mark);
        if (bucket >= 0 && bucket < CMS_GRADE_BUCKET_COUNT)
        {
            stats->grade_counts[bucket]++;
        }
    }

    stats->average = total / (float)stats->count;

    return CMS_STATUS_OK;
}

CMS_STATUS cms_display_summary(const StudentDatabase *db)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    SummaryStats stats;
    CMS_STATUS status = cms_calculate_summary(db, &stats);

    if (status == CMS_STATUS_NOT_FOUND)
    {
        printf("\nNo records available to summarize.\n\n");
        return CMS_STATUS_OK;
    }

    if (status != CMS_STATUS_OK)
    {
        return status;
    }

    printf("\nSummary Statistics:\n");
    printf("  Total Students: %zu\n", stats.count);
    printf("  Average Mark: %.2f\n", stats.average);
    printf("  Highest Mark: %.2f (ID: %d)\n", stats.highest, stats.highest_id);
    printf("  Lowest Mark: %.2f (ID: %d)\n\n", stats.lowest, stats.lowest_id);
    printf("  Grade Counts:\n");
    for (int i = 0; i < CMS_GRADE_BUCKET_COUNT; ++i)
    {
        printf("    %-3s: %zu\n", cms_grade_labels[i], stats.grade_counts[i]);
    }
    printf("\n");

    return CMS_STATUS_OK;
}

CMS_STATUS cms_show_summary(const StudentDatabase *db)
{
    return cms_display_summary(db);
}

CMS_STATUS cms_show_all(const StudentDatabase *db)
{
    return cms_database_show_all(db);
}

CMS_STATUS cms_show_all_sorted(const StudentDatabase *db, CmsSortKey sort_key, CmsSortOrder sort_order)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        printf("\nNo records available.\n\n");
        return CMS_STATUS_OK;
    }

    StudentRecord *buffer = malloc(db->count * sizeof(StudentRecord));
    if (buffer == NULL)
    {
        return CMS_STATUS_ERROR;
    }

    memcpy(buffer, db->records, db->count * sizeof(StudentRecord));

    int (*cmp)(const void *, const void *) = NULL;
    if (sort_key == CMS_SORT_KEY_ID)
    {
        cmp = (sort_order == CMS_SORT_DESC) ? compare_by_id_desc : compare_by_id_asc;
    }
    else if (sort_key == CMS_SORT_KEY_MARK)
    {
        cmp = (sort_order == CMS_SORT_DESC) ? compare_by_mark_desc : compare_by_mark_asc;
    }
    else if (sort_key == CMS_SORT_KEY_NAME)
    {
        cmp = (sort_order == CMS_SORT_DESC) ? compare_by_name_desc : compare_by_name_asc;
    }
    else if (sort_key == CMS_SORT_KEY_PROGRAMME)
    {
        cmp = (sort_order == CMS_SORT_DESC) ? compare_by_programme_desc : compare_by_programme_asc;
    }
    else
    {
        free(buffer);
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    qsort(buffer, db->count, sizeof(StudentRecord), cmp);

    StudentDatabase view = *db;
    view.records = buffer;
    view.count = db->count;
    view.capacity = db->count;

    CMS_STATUS status = cms_database_show_all(&view);
    free(buffer);
    return status;
}
