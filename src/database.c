#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"
#include "../include/config.h"
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

    /* TODO: Open file, parse records, populate database */
    return CMS_STATUS_NOT_IMPLEMENTED;
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
    return CMS_STATUS_NOT_IMPLEMENTED;
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

