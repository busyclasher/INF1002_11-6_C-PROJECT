#ifndef CMS_SUMMARY_H
#define CMS_SUMMARY_H

#include "cms.h"

typedef enum CmsSortKey
{
    CMS_SORT_KEY_NONE = 0,
    CMS_SORT_KEY_ID,
    CMS_SORT_KEY_MARK,
    CMS_SORT_KEY_NAME,
    CMS_SORT_KEY_PROGRAMME
} CmsSortKey;

typedef enum CmsSortOrder
{
    CMS_SORT_ASC = 0,
    CMS_SORT_DESC
} CmsSortOrder;

/* Sort order enumeration */
typedef enum
{
    SORT_ASCENDING,
    SORT_DESCENDING
} SortOrder;

/* Sorting functions */
CMS_STATUS cms_sort_by_id(StudentDatabase *db, SortOrder order);
CMS_STATUS cms_sort_by_name(StudentDatabase *db, SortOrder order);
CMS_STATUS cms_sort_by_prog(StudentDatabase *db, SortOrder order);
CMS_STATUS cms_sort_by_mark(StudentDatabase *db, SortOrder order);

/* Summary statistics */
typedef enum
{
    CMS_GRADE_A_PLUS = 0,
    CMS_GRADE_A,
    CMS_GRADE_B_PLUS,
    CMS_GRADE_B,
    CMS_GRADE_C_PLUS,
    CMS_GRADE_C,
    CMS_GRADE_D,
    CMS_GRADE_F,
    CMS_GRADE_BUCKET_COUNT
} CmsGradeBucket;

typedef struct
{
    size_t count;
    float average;
    float highest;
    float lowest;
    int highest_id;
    int lowest_id;
    size_t grade_counts[CMS_GRADE_BUCKET_COUNT];
} SummaryStats;

CMS_STATUS cms_calculate_summary(const StudentDatabase *db, SummaryStats *stats);
CMS_STATUS cms_display_summary(const StudentDatabase *db);
CMS_STATUS cms_show_summary(const StudentDatabase *db);
CMS_STATUS cms_show_all(const StudentDatabase *db);
CMS_STATUS cms_show_all_sorted(const StudentDatabase *db, CmsSortKey sort_key, CmsSortOrder sort_order);

#endif /* CMS_SUMMARY_H */
