#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"
#include "../include/config.h"
#include "../include/utils.h"
#include "../include/summary.h"

CMS_STATUS cms_database_init(StudentDatabase *db) {
    /* TODO: Implement database initialization */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    db->records = NULL;
    db->count = 0;
    db->capacity = 0;
    db->file_path[0] = '\0';
    db->is_loaded = false;
    db->is_dirty = false;

    return CMS_STATUS_OK;
}

void cms_database_cleanup(StudentDatabase *db) {
    /* TODO: Implement database cleanup */
    if (db != NULL && db->records != NULL) {
        free(db->records);
        db->records = NULL;
        db->count = 0;
        db->capacity = 0;
    }
}

CMS_STATUS cms_database_load(StudentDatabase *db, const char *file_path) {
    /* TODO: Implement database loading from file */
    if (db == NULL || file_path == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        return CMS_STATUS_IO;
    }

    char line[1024];
    size_t line_no = 0;

    /* Ensure database has initial capacity */
    if (db->records == NULL) {
        db->capacity = CMS_INITIAL_CAPACITY;
        db->records = (StudentRecord *)malloc(db->capacity * sizeof(StudentRecord));
        if (db->records == NULL) {
            fclose(fp);
            return CMS_STATUS_ERROR;
        }
        db->count = 0;
    }

    /* Read file line-by-line. Skip first two header lines if present. */
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_no++;

        /* Trim whitespace */
        cms_trim_string(line);

        /* Skip empty lines */
        if (line[0] == '\0') {
            continue;
        }

        /* Skip header lines (common in sample format) */
        if (line_no == 1) {
            /* e.g. "Table Name: StudentRecords" */
            continue;
        }
        if (line_no == 2) {
            /* header row: "ID\tName\tProgramme\tMark" */
            continue;
        }

        /* Tokenize by tab characters (fields expected: ID, Name, Programme, Mark) */
        char *saveptr = NULL;
        char *tok = NULL;

        tok = strtok_r(line, "\t", &saveptr);
        if (tok == NULL) {
            fclose(fp);
            return CMS_STATUS_PARSE_ERROR;
        }
        cms_trim_string(tok);
        int id = atoi(tok);
        if (!cms_validate_student_id(id)) {
            fclose(fp);
            return CMS_STATUS_PARSE_ERROR;
        }

        char name_buf[CMS_MAX_NAME_LEN + 1] = {0};
        tok = strtok_r(NULL, "\t", &saveptr);
        if (tok == NULL) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }
        cms_trim_string(tok);
        strncpy(name_buf, tok, CMS_MAX_NAME_LEN);
        name_buf[CMS_MAX_NAME_LEN] = '\0';
        if (!cms_validate_name(name_buf)) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }

        char prog_buf[CMS_MAX_PROGRAMME_LEN + 1] = {0};
        tok = strtok_r(NULL, "\t", &saveptr);
        if (tok == NULL) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }
        cms_trim_string(tok);
        strncpy(prog_buf, tok, CMS_MAX_PROGRAMME_LEN);
        prog_buf[CMS_MAX_PROGRAMME_LEN] = '\0';
        if (!cms_validate_programme(prog_buf)) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }

        tok = strtok_r(NULL, "\t\n", &saveptr);
        if (tok == NULL) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }
        cms_trim_string(tok);
        char *endptr = NULL;
        float mark = strtof(tok, &endptr);
        if (endptr == tok) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }
        if (!cms_validate_mark(mark)) { fclose(fp); return CMS_STATUS_PARSE_ERROR; }

        /* Grow array if needed */
        if (db->count >= db->capacity) {
            size_t new_cap = db->capacity * CMS_GROWTH_FACTOR;
            StudentRecord *new_mem = (StudentRecord *)realloc(db->records, new_cap * sizeof(StudentRecord));
            if (new_mem == NULL) {
                fclose(fp);
                return CMS_STATUS_ERROR;
            }
            db->records = new_mem;
            db->capacity = new_cap;
        }

        /* Append record */
        StudentRecord *rec = &db->records[db->count];
        rec->id = id;
        strncpy(rec->name, name_buf, CMS_MAX_NAME_LEN);
        rec->name[CMS_MAX_NAME_LEN] = '\0';
        strncpy(rec->programme, prog_buf, CMS_MAX_PROGRAMME_LEN);
        rec->programme[CMS_MAX_PROGRAMME_LEN] = '\0';
        rec->mark = mark;

        db->count++;
    }

    fclose(fp);

    /* Store file path and flags */
    strncpy(db->file_path, file_path, CMS_MAX_FILE_PATH_LEN - 1);
    db->file_path[CMS_MAX_FILE_PATH_LEN - 1] = '\0';
    db->is_loaded = true;
    db->is_dirty = false;
    printf("Loaded %zu record(s) from '%s'\n", db->count, db->file_path);
    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_save(StudentDatabase *db, const char *file_path) {
    /* TODO: Implement database saving to file */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Write records to file in proper format */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cms_database_insert(StudentDatabase *db, const StudentRecord *record) {
    /* TODO: Implement record insertion */
    if (db == NULL || record == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Check for duplicate ID */
    /* TODO: Resize array if needed */
    /* TODO: Add record to database */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cms_database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record) {
    /* TODO: Implement record query */
    if (db == NULL || out_record == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Search for record by ID */
    return CMS_STATUS_NOT_FOUND;
}

CMS_STATUS cms_database_update(StudentDatabase *db, int student_id, const StudentRecord *new_record) {
    /* TODO: Implement record update */
    if (db == NULL || new_record == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Find record and update fields */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cms_database_delete(StudentDatabase *db, int student_id) {
    /* TODO: Implement record deletion */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Find and remove record */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cms_database_show_all(const StudentDatabase *db) {
    /* TODO: Implement display of all records */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Print all records in formatted table */
    if (db->records == NULL || db->count == 0) {
        printf("No records to display.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    } else {
        cms_display_table(db);
        return CMS_STATUS_OK;
    }
}

CMS_STATUS cms_database_show_sorted(const StudentDatabase *db, char sort_key[32], char sort_order[32]) {
    /* Display sorted records without modifying the original database */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
        // return CMS_STATUS_NOT_FOUND;
    }

    if (db->records == NULL || db->count == 0) {
        printf("No records to display.\n");
        return CMS_STATUS_OK;
    }

    /* Create a copy of the records to sort */
    StudentRecord *sorted_records = (StudentRecord *)malloc(db->count * sizeof(StudentRecord));
    if (sorted_records == NULL) {
        return CMS_STATUS_ERROR;
    }

    /* Copy records */
    memcpy(sorted_records, db->records, db->count * sizeof(StudentRecord));

    /* Note: Sorting logic would use cms_sort_by_id() or cms_sort_by_mark() from summary.h
       For now, we'll just display records in current order */
    
    /* Display header */
    printf("\nTable Name: StudentRecords\n");
    printf("%-12s %-20s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    printf("%-12s %-20s %-30s %-10s\n", "----", "----", "---------", "----");

    /* Display sorted records */
    for (size_t i = 0; i < db->count; i++) {
        printf("%-12d %-20s %-30s %-10.1f\n",
               sorted_records[i].id,
               sorted_records[i].name,
               sorted_records[i].programme,
               sorted_records[i].mark);
    }
    printf("\n");

    /* Free the copied records */
    free(sorted_records);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_show_record(const StudentRecord *record) {
    /* TODO: Implement single record display */
    if (record == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Print record details */
    printf("ID: %d, Name: %s, Programme: %s, Mark: %.2f\n",
           record->id, record->name, record->programme, record->mark);

    return CMS_STATUS_OK;
}

