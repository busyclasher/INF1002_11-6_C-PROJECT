#include "summary.h"

CmsStatus cms_sort_records(StudentDatabase *db, CmsSortKey key, CmsSortOrder order, bool in_place) {
    (void)db;
    (void)key;
    (void)order;
    (void)in_place;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus cms_show_all(const StudentDatabase *db) {
    (void)db;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus cms_show_all_sorted(StudentDatabase *db, CmsSortKey key, CmsSortOrder order) {
    (void)db;
    (void)key;
    (void)order;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus cms_show_summary(const StudentDatabase *db) {
    (void)db;
    return CMS_STATUS_NOT_IMPLEMENTED;
}
