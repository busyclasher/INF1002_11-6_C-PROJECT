#include "database.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define CMS_INITIAL_CAPACITY 8

static CmsStatus database_ensure_capacity(StudentDatabase *db, size_t min_capacity);
static int database_find_index(const StudentDatabase *db, int student_id);
static CmsStatus database_add_record(StudentDatabase *db,
                                     const StudentRecord *record,
                                     bool fail_on_duplicate,
                                     bool mark_dirty);
static CmsStatus database_validate_record(const StudentRecord *record);
static CmsStatus database_parse_record_line(const char *line, StudentRecord *out_record);
static void database_move(StudentDatabase *dest, StudentDatabase *src);

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
    free(db->records);
    db->records = NULL;
    db->count = 0;
    db->capacity = 0;
}

CmsStatus database_set_file(StudentDatabase *db, const char *file_path) {
    if (!db || !file_path || file_path[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    size_t length = strlen(file_path);
    if (length >= CMS_MAX_FILE_PATH_LEN) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    strncpy(db->file_path, file_path, CMS_MAX_FILE_PATH_LEN);
    db->file_path[CMS_MAX_FILE_PATH_LEN - 1] = '\0';
    return CMS_STATUS_OK;
}

const char *database_get_file(const StudentDatabase *db) {
    if (!db) {
        return NULL;
    }
    return db->file_path[0] ? db->file_path : NULL;
}

bool database_is_loaded(const StudentDatabase *db) {
    return db && db->is_loaded;
}

bool database_is_dirty(const StudentDatabase *db) {
    return db && db->is_dirty;
}

CmsStatus database_open(StudentDatabase *db, const char *file_path) {
    if (!db || !file_path || file_path[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    FILE *file = fopen(file_path, "r");
    if (!file) {
        return CMS_STATUS_IO;
    }

    StudentDatabase temp;
    database_init(&temp);
    CmsStatus status = database_set_file(&temp, file_path);
    if (status != CMS_STATUS_OK) {
        fclose(file);
        database_free(&temp);
        return status;
    }

    char line[CMS_MAX_LINE_LEN];
    bool header_seen = false;

    while (fgets(line, sizeof(line), file)) {
        cms_trim(line);

        if (line[0] == '\0') {
            continue;
        }

        if (!header_seen) {
            if (strstr(line, "ID") && strstr(line, "Mark")) {
                header_seen = true;
            }
            continue;
        }

        StudentRecord record;
        status = database_parse_record_line(line, &record);
        if (status != CMS_STATUS_OK) {
            fclose(file);
            database_free(&temp);
            return CMS_STATUS_PARSE_ERROR;
        }

        status = database_add_record(&temp, &record, true, false);
        if (status != CMS_STATUS_OK) {
            fclose(file);
            database_free(&temp);
            return status;
        }
    }

    fclose(file);

    if (!header_seen) {
        database_free(&temp);
        return CMS_STATUS_PARSE_ERROR;
    }

    temp.is_loaded = true;
    temp.is_dirty = false;

    database_move(db, &temp);
    return CMS_STATUS_OK;
}

CmsStatus database_save(StudentDatabase *db, const char *file_path) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!db->is_loaded) {
        return CMS_STATUS_ERROR;
    }

    if (file_path && file_path[0] != '\0') {
        CmsStatus status = database_set_file(db, file_path);
        if (status != CMS_STATUS_OK) {
            return status;
        }
    }

    const char *target_path = database_get_file(db);
    if (!target_path) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    FILE *file = fopen(target_path, "w");
    if (!file) {
        return CMS_STATUS_IO;
    }

    fprintf(file, "Table Name: StudentRecords\n");
    fprintf(file, "ID\tName\tProgramme\tMark\n");

    for (size_t i = 0; i < db->count; ++i) {
        const StudentRecord *record = &db->records[i];
        fprintf(file, "%d\t%s\t%s\t%.2f\n",
                record->id,
                record->name,
                record->programme,
                record->mark);
    }

    if (fclose(file) == EOF) {
        return CMS_STATUS_IO;
    }

    db->is_dirty = false;
    return CMS_STATUS_OK;
}

CmsStatus database_insert(StudentDatabase *db, const StudentRecord *record) {
    if (!db || !record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!db->is_loaded) {
        return CMS_STATUS_ERROR;
    }
    return database_add_record(db, record, true, true);
}

CmsStatus database_update(StudentDatabase *db, const StudentRecord *record) {
    if (!db || !record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!db->is_loaded) {
        return CMS_STATUS_ERROR;
    }
    CmsStatus status = database_validate_record(record);
    if (status != CMS_STATUS_OK) {
        return status;
    }

    int index = database_find_index(db, record->id);
    if (index < 0) {
        return CMS_STATUS_NOT_FOUND;
    }

    db->records[index] = *record;
    db->is_dirty = true;
    return CMS_STATUS_OK;
}

CmsStatus database_delete(StudentDatabase *db, int student_id, bool *deleted) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!db->is_loaded) {
        if (deleted) {
            *deleted = false;
        }
        return CMS_STATUS_ERROR;
    }

    int index = database_find_index(db, student_id);
    if (index < 0) {
        if (deleted) {
            *deleted = false;
        }
        return CMS_STATUS_NOT_FOUND;
    }

    if (index < (int)(db->count - 1)) {
        memmove(&db->records[index],
                &db->records[index + 1],
                (db->count - index - 1) * sizeof(StudentRecord));
    }
    --db->count;
    db->is_dirty = true;
    if (deleted) {
        *deleted = true;
    }
    return CMS_STATUS_OK;
}

CmsStatus database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record) {
    if (!db || !out_record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!db->is_loaded) {
        return CMS_STATUS_ERROR;
    }
    int index = database_find_index(db, student_id);
    if (index < 0) {
        return CMS_STATUS_NOT_FOUND;
    }
    *out_record = db->records[index];
    return CMS_STATUS_OK;
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

static CmsStatus database_copy_file_path(StudentDatabase *db, const char *file_path) {
    return database_set_file(db, file_path);
}

static CmsStatus database_ensure_capacity(StudentDatabase *db, size_t min_capacity) {
    if (db->capacity >= min_capacity) {
        return CMS_STATUS_OK;
    }

    size_t new_capacity = db->capacity == 0 ? CMS_INITIAL_CAPACITY : db->capacity * 2;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }

    StudentRecord *new_records = realloc(db->records, new_capacity * sizeof(StudentRecord));
    if (!new_records) {
        return CMS_STATUS_ERROR;
    }

    db->records = new_records;
    db->capacity = new_capacity;
    return CMS_STATUS_OK;
}

static int database_find_index(const StudentDatabase *db, int student_id) {
    if (!db || !db->records) {
        return -1;
    }
    for (size_t i = 0; i < db->count; ++i) {
        if (db->records[i].id == student_id) {
            return (int)i;
        }
    }
    return -1;
}

static CmsStatus database_add_record(StudentDatabase *db,
                                     const StudentRecord *record,
                                     bool fail_on_duplicate,
                                     bool mark_dirty) {
    CmsStatus status = database_validate_record(record);
    if (status != CMS_STATUS_OK) {
        return status;
    }

    int index = database_find_index(db, record->id);
    if (index >= 0) {
        if (fail_on_duplicate) {
            return CMS_STATUS_DUPLICATE;
        }
        db->records[index] = *record;
        if (mark_dirty) {
            db->is_dirty = true;
        }
        return CMS_STATUS_OK;
    }

    status = database_ensure_capacity(db, db->count + 1);
    if (status != CMS_STATUS_OK) {
        return status;
    }

    db->records[db->count++] = *record;
    if (mark_dirty) {
        db->is_dirty = true;
    }

    return CMS_STATUS_OK;
}

static CmsStatus database_validate_record(const StudentRecord *record) {
    if (!record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (record->id <= 0) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (record->name[0] == '\0' || record->programme[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (record->mark < CMS_MIN_MARK || record->mark > CMS_MAX_MARK) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(record->name) > CMS_MAX_NAME_LEN) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(record->programme) > CMS_MAX_PROGRAMME_LEN) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    return CMS_STATUS_OK;
}

static CmsStatus database_parse_record_line(const char *line, StudentRecord *out_record) {
    if (!line || !out_record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_LINE_LEN];
    strncpy(buffer, line, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *fields[4] = {0};
    size_t field_count = 0;

    char *cursor = buffer;
    while (*cursor == '\t') {
        ++cursor;
    }

    while (*cursor && field_count < 4) {
        fields[field_count++] = cursor;
        while (*cursor && *cursor != '\t') {
            ++cursor;
        }
        if (*cursor == '\t') {
            *cursor = '\0';
            ++cursor;
            while (*cursor == '\t') {
                ++cursor;
            }
        }
    }

    if (field_count != 4 || *cursor != '\0') {
        return CMS_STATUS_PARSE_ERROR;
    }

    for (size_t i = 0; i < field_count; ++i) {
        cms_trim(fields[i]);
        if (fields[i][0] == '\0') {
            return CMS_STATUS_PARSE_ERROR;
        }
    }

    char *endptr = NULL;
    errno = 0;
    long id_long = strtol(fields[0], &endptr, 10);
    if (errno != 0 || endptr == fields[0] || *endptr != '\0' || id_long <= 0) {
        return CMS_STATUS_PARSE_ERROR;
    }

    errno = 0;
    float mark = strtof(fields[3], &endptr);
    if (errno != 0 || endptr == fields[3] || *endptr != '\0') {
        return CMS_STATUS_PARSE_ERROR;
    }

    if (strlen(fields[1]) > CMS_MAX_NAME_LEN || strlen(fields[2]) > CMS_MAX_PROGRAMME_LEN) {
        return CMS_STATUS_PARSE_ERROR;
    }

    out_record->id = (int)id_long;
    strncpy(out_record->name, fields[1], sizeof(out_record->name));
    out_record->name[sizeof(out_record->name) - 1] = '\0';
    strncpy(out_record->programme, fields[2], sizeof(out_record->programme));
    out_record->programme[sizeof(out_record->programme) - 1] = '\0';
    out_record->mark = mark;

    return database_validate_record(out_record);
}

static void database_move(StudentDatabase *dest, StudentDatabase *src) {
    if (!dest || !src) {
        return;
    }
    free(dest->records);
    *dest = *src;
    src->records = NULL;
    src->count = 0;
    src->capacity = 0;
    src->is_loaded = false;
    src->is_dirty = false;
    if (sizeof(src->file_path) > 0) {
        src->file_path[0] = '\0';
    }
}
