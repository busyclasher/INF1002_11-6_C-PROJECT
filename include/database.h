#ifndef CMS_DATABASE_H
#define CMS_DATABASE_H

#include "cms.h"
#include "summary.h"

/* Database initialization and cleanup */
CMS_STATUS cms_database_init(StudentDatabase *db);
void cms_database_cleanup(StudentDatabase *db);

/* Database file operations */
CMS_STATUS cms_database_load(StudentDatabase *db, const char *file_path);
CMS_STATUS cms_database_save(StudentDatabase *db, const char *file_path);

/* Record operations */
CMS_STATUS cms_database_insert(StudentDatabase *db, const StudentRecord *record);
CMS_STATUS cms_database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record);
CMS_STATUS cms_database_update(StudentDatabase *db, int student_id, const StudentRecord *new_record);
CMS_STATUS cms_database_delete(StudentDatabase *db, int student_id);

/* Display operations */
CMS_STATUS cms_database_show_all(const StudentDatabase *db);
CMS_STATUS cms_database_show_record(const StudentRecord *record);
CMS_STATUS cms_database_show_sorted(const StudentDatabase *db, CmsSortKey sort_key, CmsSortOrder sort_order);

#endif /* CMS_DATABASE_H */
