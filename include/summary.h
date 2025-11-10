#ifndef CMS_SUMMARY_H
#define CMS_SUMMARY_H

#include "cms.h"

/* Sort order enumeration */
typedef enum {
    SORT_ASCENDING,
    SORT_DESCENDING
} SortOrder;

/* Sorting functions */
CMS_STATUS cms_sort_by_id(StudentDatabase *db, SortOrder order);
CMS_STATUS cms_sort_by_mark(StudentDatabase *db, SortOrder order);

/* Summary statistics */
typedef struct {
    size_t count;
    float average;
    float highest;
    float lowest;
    int highest_id;
    int lowest_id;
} SummaryStats;

CMS_STATUS cms_calculate_summary(const StudentDatabase *db, SummaryStats *stats);
CMS_STATUS cms_display_summary(const StudentDatabase *db);

#endif /* CMS_SUMMARY_H */

