#include <stdio.h>
#include <stdlib.h>
#include "../include/summary.h"

/* Comparison functions for qsort */
static int compare_by_id_asc(const void *a, const void *b) {
    /* TODO: Implement ID ascending comparison */
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;
    return rec_a->id - rec_b->id;
}

static int compare_by_id_desc(const void *a, const void *b) {
    /* TODO: Implement ID descending comparison */
    return compare_by_id_asc(b, a);
}

static int compare_by_mark_asc(const void *a, const void *b) {
    /* TODO: Implement mark ascending comparison */
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;
    
    if (rec_a->mark < rec_b->mark) return -1;
    if (rec_a->mark > rec_b->mark) return 1;
    return 0;
}

static int compare_by_mark_desc(const void *a, const void *b) {
    /* TODO: Implement mark descending comparison */
    return compare_by_mark_asc(b, a);
}

CMS_STATUS cms_sort_by_id(StudentDatabase *db, SortOrder order) {
    /* TODO: Implement sorting by ID */
    if (db == NULL || db->records == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING) {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_asc);
    } else {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_sort_by_mark(StudentDatabase *db, SortOrder order) {
    /* TODO: Implement sorting by mark */
    if (db == NULL || db->records == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING) {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_asc);
    } else {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_calculate_summary(const StudentDatabase *db, SummaryStats *stats) {
    /* TODO: Implement summary calculation */
    if (db == NULL || stats == NULL || db->count == 0) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Calculate count, average, highest, lowest */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cms_display_summary(const StudentDatabase *db) {
    /* TODO: Implement summary display */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    SummaryStats stats;
    CMS_STATUS status = cms_calculate_summary(db, &stats);
    
    if (status == CMS_STATUS_OK) {
        /* TODO: Print formatted summary */
        printf("\nSummary Statistics:\n");
        printf("  Total Students: %zu\n", stats.count);
        printf("  Average Mark: %.2f\n", stats.average);
        printf("  Highest Mark: %.2f (ID: %d)\n", stats.highest, stats.highest_id);
        printf("  Lowest Mark: %.2f (ID: %d)\n\n", stats.lowest, stats.lowest_id);
    }

    return status;
}

