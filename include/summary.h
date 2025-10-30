#ifndef SUMMARY_H
#define SUMMARY_H

#include <stdbool.h>

#include "cms.h"
#include "database.h"

typedef enum CmsSortKey {
    CMS_SORT_KEY_NONE = 0,
    CMS_SORT_KEY_ID,
    CMS_SORT_KEY_MARK
} CmsSortKey;

typedef enum CmsSortOrder {
    CMS_SORT_ASC = 0,
    CMS_SORT_DESC
} CmsSortOrder;

CmsStatus cms_show_all(const StudentDatabase *db);
CmsStatus cms_show_all_sorted(const StudentDatabase *db, CmsSortKey key, CmsSortOrder order);
CmsStatus cms_show_summary(const StudentDatabase *db);
CmsStatus cms_show_programme_stats(const StudentDatabase *db);

#endif /* SUMMARY_H */
