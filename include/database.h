#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <stddef.h>

#include "cms.h"
#include "config.h"

typedef struct StudentRecord {
    int id;
    char name[CMS_MAX_NAME_LEN + 1];
    char programme[CMS_MAX_PROGRAMME_LEN + 1];
    float mark;
} StudentRecord;

typedef struct StudentDatabase {
    StudentRecord *records;
    size_t count;
    size_t capacity;
    char file_path[CMS_MAX_FILE_PATH_LEN];
    bool is_loaded;
    bool is_dirty;
} StudentDatabase;

CmsStatus database_init(StudentDatabase *db);
void database_free(StudentDatabase *db);
void database_reset(StudentDatabase *db);

CmsStatus database_set_file(StudentDatabase *db, const char *file_path);
const char *database_get_file(const StudentDatabase *db);

bool database_is_loaded(const StudentDatabase *db);
bool database_is_dirty(const StudentDatabase *db);

CmsStatus database_open(StudentDatabase *db, const char *file_path);
CmsStatus database_save(StudentDatabase *db, const char *file_path);

CmsStatus database_insert(StudentDatabase *db, const StudentRecord *record);
CmsStatus database_update(StudentDatabase *db, const StudentRecord *record);
CmsStatus database_delete(StudentDatabase *db, int student_id, bool *deleted);
CmsStatus database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record);

const StudentRecord *database_records(const StudentDatabase *db);
size_t database_record_count(const StudentDatabase *db);

#endif /* DATABASE_H */
