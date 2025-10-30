#include "database.h"

#include <stdlib.h>
#include <string.h>

CmsStatus database_init(StudentDatabase *db) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    memset(db, 0, sizeof(*db));
    return CMS_STATUS_OK;
}

void database_reset(StudentDatabase *db) {
    if (!db) {
        return;
    }
    free(db->records);
    db->records = NULL;
    db->count = 0;
    db->capacity = 0;
    db->is_loaded = false;
    db->is_dirty = false;
    if (sizeof(db->file_path) > 0) {
        db->file_path[0] = '\0';
    }
}

void database_free(StudentDatabase *db) {
    if (!db) {
        return;
    }
    database_reset(db);
}

CmsStatus database_set_file(StudentDatabase *db, const char *file_path) {
    (void)db;
    (void)file_path;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

const char *database_get_file(const StudentDatabase *db) {
    if (!db) {
        return NULL;
    }
    return db->file_path;
}

bool database_is_loaded(const StudentDatabase *db) {
    if (!db) {
        return false;
    }
    return db->is_loaded;
}

bool database_is_dirty(const StudentDatabase *db) {
    if (!db) {
        return false;
    }
    return db->is_dirty;
}

CmsStatus database_open(StudentDatabase *db, const char *file_path) {
    (void)db;
    (void)file_path;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus database_save(const StudentDatabase *db, const char *file_path) {
    (void)db;
    (void)file_path;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus database_insert(StudentDatabase *db, const StudentRecord *record) {
    (void)db;
    (void)record;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus database_update(StudentDatabase *db, const StudentRecord *record) {
    (void)db;
    (void)record;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus database_delete(StudentDatabase *db, int student_id, bool *deleted) {
    (void)db;
    (void)student_id;
    if (deleted) {
        *deleted = false;
    }
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record) {
    (void)db;
    (void)student_id;
    (void)out_record;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

const StudentRecord *database_records(const StudentDatabase *db) {
    if (!db) {
        return NULL;
    }
    return db->records;
}

size_t database_record_count(const StudentDatabase *db) {
    if (!db) {
        return 0;
    }
    return db->count;
}
