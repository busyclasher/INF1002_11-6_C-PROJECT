#include "summary.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMS_PRINT_ID_WIDTH 10
#define CMS_PRINT_NAME_WIDTH 24
#define CMS_PRINT_PROGRAMME_WIDTH 28

typedef struct ProgrammeStats {
    char name[CMS_MAX_PROGRAMME_LEN + 1];
    size_t count;
    double total_mark;
} ProgrammeStats;

static CmsSortKey g_sort_key = CMS_SORT_KEY_NONE;
static CmsSortOrder g_sort_order = CMS_SORT_ASC;

static void print_records_table(const StudentRecord *records, size_t count);
static int compare_records(const void *lhs, const void *rhs);
static ProgrammeStats *find_programme_stat(ProgrammeStats *stats, size_t count, const char *name);

CmsStatus cms_show_all(const StudentDatabase *db) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!database_is_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    size_t count = database_record_count(db);
    const StudentRecord *records = database_records(db);

    if (!records || count == 0) {
        puts("CMS: No records available in StudentRecords.");
        return CMS_STATUS_OK;
    }

    print_records_table(records, count);
    return CMS_STATUS_OK;
}

CmsStatus cms_show_all_sorted(const StudentDatabase *db, CmsSortKey key, CmsSortOrder order) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!database_is_loaded(db)) {
        return CMS_STATUS_ERROR;
    }
    if (key == CMS_SORT_KEY_NONE) {
        return cms_show_all(db);
    }

    size_t count = database_record_count(db);
    const StudentRecord *records = database_records(db);
    if (!records || count == 0) {
        puts("CMS: No records available in StudentRecords.");
        return CMS_STATUS_OK;
    }

    StudentRecord *sorted = malloc(count * sizeof(StudentRecord));
    if (!sorted) {
        return CMS_STATUS_ERROR;
    }
    memcpy(sorted, records, count * sizeof(StudentRecord));

    g_sort_key = key;
    g_sort_order = order;
    qsort(sorted, count, sizeof(StudentRecord), compare_records);

    print_records_table(sorted, count);

    free(sorted);
    return CMS_STATUS_OK;
}

CmsStatus cms_show_summary(const StudentDatabase *db) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!database_is_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    size_t count = database_record_count(db);
    const StudentRecord *records = database_records(db);
    if (!records || count == 0) {
        puts("CMS: No records available to summarise.");
        return CMS_STATUS_OK;
    }

    double total = 0.0;
    float highest_mark = records[0].mark;
    float lowest_mark = records[0].mark;

    for (size_t i = 0; i < count; ++i) {
        total += records[i].mark;
        if (records[i].mark > highest_mark) {
            highest_mark = records[i].mark;
        }
        if (records[i].mark < lowest_mark) {
            lowest_mark = records[i].mark;
        }
    }

    double average = total / (double)count;

    printf("CMS: Total number of students: %zu\n", count);
    printf("CMS: Average mark: %.2f\n", average);

    printf("CMS: Highest mark: %.2f by ", highest_mark);
    bool first = true;
    for (size_t i = 0; i < count; ++i) {
        if (records[i].mark == highest_mark) {
            if (!first) {
                fputs(", ", stdout);
            }
            fputs(records[i].name, stdout);
            first = false;
        }
    }
    puts("");

    printf("CMS: Lowest mark: %.2f by ", lowest_mark);
    first = true;
    for (size_t i = 0; i < count; ++i) {
        if (records[i].mark == lowest_mark) {
            if (!first) {
                fputs(", ", stdout);
            }
            fputs(records[i].name, stdout);
            first = false;
        }
    }
    puts("");

    return CMS_STATUS_OK;
}

CmsStatus cms_show_programme_stats(const StudentDatabase *db) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!database_is_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    size_t count = database_record_count(db);
    const StudentRecord *records = database_records(db);
    if (!records || count == 0) {
        puts("CMS: No records available to display programme statistics.");
        return CMS_STATUS_OK;
    }

    ProgrammeStats *stats = calloc(count, sizeof(ProgrammeStats));
    if (!stats) {
        return CMS_STATUS_ERROR;
    }

    size_t programme_count = 0;
    for (size_t i = 0; i < count; ++i) {
        ProgrammeStats *stat = find_programme_stat(stats, programme_count, records[i].programme);
        if (!stat) {
            stat = &stats[programme_count++];
            strncpy(stat->name, records[i].programme, sizeof(stat->name) - 1);
            stat->name[sizeof(stat->name) - 1] = '\0';
        }
        stat->count += 1;
        stat->total_mark += records[i].mark;
    }

    printf("%-*s %-10s %-10s\n",
           CMS_PRINT_PROGRAMME_WIDTH, "Programme",
           "Students",
           "Avg Mark");
    for (size_t i = 0; i < programme_count; ++i) {
        double avg = stats[i].total_mark / (double)stats[i].count;
        printf("%-*.*s %-10zu %10.2f\n",
               CMS_PRINT_PROGRAMME_WIDTH, CMS_PRINT_PROGRAMME_WIDTH,
               stats[i].name,
               stats[i].count,
               avg);
    }

    size_t best_index = 0;
    double best_avg = stats[0].total_mark / (double)stats[0].count;
    for (size_t i = 1; i < programme_count; ++i) {
        double avg = stats[i].total_mark / (double)stats[i].count;
        if (avg > best_avg) {
            best_avg = avg;
            best_index = i;
        }
    }

    printf("CMS: Programme with highest average mark: %s (%.2f)\n",
           stats[best_index].name,
           best_avg);

    free(stats);
    return CMS_STATUS_OK;
}

static void print_records_table(const StudentRecord *records, size_t count) {
    printf("%-*s %-*s %-*s %s\n",
           CMS_PRINT_ID_WIDTH, "ID",
           CMS_PRINT_NAME_WIDTH, "Name",
           CMS_PRINT_PROGRAMME_WIDTH, "Programme",
           "Mark");
    for (size_t i = 0; i < count; ++i) {
        printf("%-*d %-*.*s %-*.*s %6.2f\n",
               CMS_PRINT_ID_WIDTH, records[i].id,
               CMS_PRINT_NAME_WIDTH, CMS_PRINT_NAME_WIDTH, records[i].name,
               CMS_PRINT_PROGRAMME_WIDTH, CMS_PRINT_PROGRAMME_WIDTH, records[i].programme,
               records[i].mark);
    }
}

static int compare_records(const void *lhs, const void *rhs) {
    const StudentRecord *a = (const StudentRecord *)lhs;
    const StudentRecord *b = (const StudentRecord *)rhs;

    int result = 0;
    switch (g_sort_key) {
    case CMS_SORT_KEY_ID:
        if (a->id < b->id) {
            result = -1;
        } else if (a->id > b->id) {
            result = 1;
        }
        break;
    case CMS_SORT_KEY_MARK:
        if (a->mark < b->mark) {
            result = -1;
        } else if (a->mark > b->mark) {
            result = 1;
        }
        break;
    default:
        break;
    }

    if (g_sort_order == CMS_SORT_DESC) {
        result = -result;
    }
    return result;
}

static ProgrammeStats *find_programme_stat(ProgrammeStats *stats, size_t count, const char *name) {
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(stats[i].name, name) == 0) {
            return &stats[i];
        }
    }
    return NULL;
}
