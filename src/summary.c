#include "summary.h"

#include <stdio.h>

CmsStatus cms_sort_records(StudentDatabase *db, CmsSortKey key, CmsSortOrder order, bool in_place) {
    (void)db;
    (void)key;
    (void)order;
    (void)in_place;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

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

    printf("%-10s %-24s %-28s %s\n", "ID", "Name", "Programme", "Mark");
    for (size_t i = 0; i < count; ++i) {
        printf("%-10d %-24.24s %-28.28s %6.2f\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark);
    }

    return CMS_STATUS_OK;
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
