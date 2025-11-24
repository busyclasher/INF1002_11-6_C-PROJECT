#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"
#include "../include/config.h"
#include "../include/utils.h"

static void cms_clear_undo_state(StudentDatabase *db)
{
    if (db == NULL)
    {
        return;
    }
    db->undo_state.action = CMS_UNDO_NONE;
    db->undo_state.valid = false;
    db->undo_state.index = 0;
    db->undo_state.prev_dirty = db->is_dirty;
    memset(&db->undo_state.before, 0, sizeof(StudentRecord));
    memset(&db->undo_state.after, 0, sizeof(StudentRecord));
}

static void cms_set_undo_state(StudentDatabase *db,
                               CmsUndoAction action,
                               const StudentRecord *before,
                               const StudentRecord *after,
                               size_t index,
                               bool prev_dirty)
{
    if (db == NULL)
    {
        return;
    }

    db->undo_state.action = action;
    db->undo_state.valid = true;
    db->undo_state.index = index;
    db->undo_state.prev_dirty = prev_dirty;

    if (before != NULL)
    {
        db->undo_state.before = *before;
    }
    else
    {
        memset(&db->undo_state.before, 0, sizeof(StudentRecord));
    }

    if (after != NULL)
    {
        db->undo_state.after = *after;
    }
    else
    {
        memset(&db->undo_state.after, 0, sizeof(StudentRecord));
    }
}

static CMS_STATUS cms_ensure_capacity(StudentDatabase *db)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->count < db->capacity)
    {
        return CMS_STATUS_OK;
    }

    size_t new_capacity = (db->capacity == 0)
                              ? CMS_INITIAL_CAPACITY
                              : db->capacity * CMS_GROWTH_FACTOR;

    StudentRecord *new_records = realloc(db->records, new_capacity * sizeof(StudentRecord));
    if (new_records == NULL)
    {
        return CMS_STATUS_ERROR;
    }

    db->records = new_records;
    db->capacity = new_capacity;
    return CMS_STATUS_OK;
}

static bool cms_database_find_index(const StudentDatabase *db, int student_id, size_t *out_index)
{
    if (db == NULL || db->records == NULL || db->count == 0)
    {
        return false;
    }

    for (size_t i = 0; i < db->count; ++i)
    {
        if (db->records[i].id == student_id)
        {
            if (out_index != NULL)
            {
                *out_index = i;
            }
            return true;
        }
    }

    return false;
}

CMS_STATUS cms_database_init(StudentDatabase *db)
{
    /* Initialize database structure and allocate initial storage.
       After successful initialization the database contains no records
       but is safe for OPEN/INSERT operations. */
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    db->records = malloc(sizeof(StudentRecord) * CMS_INITIAL_CAPACITY);
    if (db->records == NULL)
    {
        return CMS_STATUS_ERROR;
    }

    db->count = 0;
    db->capacity = CMS_INITIAL_CAPACITY;
    db->file_path[0] = '\0';
    db->is_loaded = false;
    db->is_dirty = false;
    cms_clear_undo_state(db);

    return CMS_STATUS_OK;
}

void cms_database_cleanup(StudentDatabase *db)
{
    if (db == NULL)
    {
        return;
    }

    if (db->records != NULL)
    {
        free(db->records);
        db->records = NULL;
    }

    db->count = 0;
    db->capacity = 0;
    db->file_path[0] = '\0';
    db->is_loaded = false;
    db->is_dirty = false;
    cms_clear_undo_state(db);
}

static void cms_database_reset_runtime_state(StudentDatabase *db)
{
    if (db == NULL)
    {
        return;
    }
    db->count = 0;
    db->is_loaded = false;
    db->is_dirty = false;
    cms_clear_undo_state(db);
}

CMS_STATUS cms_database_load(StudentDatabase *db, const char *file_path)
{
    if (db == NULL || file_path == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        return CMS_STATUS_IO;
    }

    // if (db != NULL){
    //     cms_database_cleanup(db);
    // }

    cms_database_reset_runtime_state(db);

    char line[CMS_MAX_COMMAND_LEN];

    /* Read and validate table name */
    if (fgets(line, sizeof(line), fp) == NULL)
    {
        fclose(fp);
        return CMS_STATUS_PARSE_ERROR;
    }

    cms_trim_string(line);
    if (strcmp(line, "Table Name: StudentRecords") != 0)
    {
        fclose(fp);
        return CMS_STATUS_PARSE_ERROR;
    }

    /* Read and validate column header line */
    if (fgets(line, sizeof(line), fp) == NULL)
    {
        fclose(fp);
        return CMS_STATUS_PARSE_ERROR;
    }

    cms_trim_string(line);
    if (strcmp(line, "ID\tName\tProgramme\tMark") != 0)
    {
        fclose(fp);
        return CMS_STATUS_PARSE_ERROR;
    }

    CMS_STATUS status = CMS_STATUS_OK;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        cms_trim_string(line);
        if (line[0] == '\0')
        {
            continue;
        }

        char *id_str = strtok(line, "\t");
        char *name = strtok(NULL, "\t");
        char *programme = strtok(NULL, "\t");
        char *mark_str = strtok(NULL, "\t");

        if (!id_str || !name || !programme || !mark_str)
        {
            status = CMS_STATUS_PARSE_ERROR;
            break;
        }

        cms_trim_string(id_str);
        cms_trim_string(name);
        cms_trim_string(programme);
        cms_trim_string(mark_str);

        int id = atoi(id_str);
        if (!cms_validate_student_id(id))
        {
            status = CMS_STATUS_PARSE_ERROR;
            break;
        }

        if (!cms_validate_name(name) || !cms_validate_programme(programme))
        {
            status = CMS_STATUS_PARSE_ERROR;
            break;
        }

        char *endptr = NULL;
        float mark = strtof(mark_str, &endptr);
        if (endptr == mark_str || !cms_validate_mark(mark))
        {
            status = CMS_STATUS_PARSE_ERROR;
            break;
        }

        status = cms_ensure_capacity(db);
        if (status != CMS_STATUS_OK)
        {
            break;
        }

        StudentRecord *record = &db->records[db->count];
        record->id = id;
        strncpy(record->name, name, CMS_MAX_NAME_LEN);
        record->name[CMS_MAX_NAME_LEN] = '\0';
        strncpy(record->programme, programme, CMS_MAX_PROGRAMME_LEN);
        record->programme[CMS_MAX_PROGRAMME_LEN] = '\0';
        record->mark = mark;
        db->count++;
    }

    printf("Loaded %zu record(s)\n", db->count);

    if (status != CMS_STATUS_OK)
    {
        cms_database_reset_runtime_state(db);
        fclose(fp);
        return status;
    }

    strncpy(db->file_path, file_path, CMS_MAX_FILE_PATH_LEN - 1);
    db->file_path[CMS_MAX_FILE_PATH_LEN - 1] = '\0';
    db->is_loaded = true;
    db->is_dirty = false;

    fclose(fp);

    /* Mark database as successfully loaded and record file path */
    db->is_loaded = true;
    db->is_dirty = false;
    strncpy(db->file_path, file_path, CMS_MAX_FILE_PATH_LEN - 1);
    db->file_path[CMS_MAX_FILE_PATH_LEN - 1] = '\0';
    cms_clear_undo_state(db);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_save(StudentDatabase *db, const char *file_path)
{
    if (db == NULL || file_path == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (file_path[0] == '\0')
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    FILE *fp = fopen(file_path, "w");
    if (fp == NULL)
    {
        return CMS_STATUS_IO;
    }

    CMS_STATUS status = CMS_STATUS_OK;

    if (fprintf(fp, "Table Name: StudentRecords\n") < 0 ||
        fprintf(fp, "ID\tName\tProgramme\tMark\n") < 0)
    {
        status = CMS_STATUS_IO;
    }
    else
    {
        for (size_t i = 0; i < db->count; i++)
        {
            const StudentRecord *rec = &db->records[i];
            if (fprintf(fp, "%d\t%s\t%s\t%.2f\n",
                        rec->id, rec->name, rec->programme, rec->mark) < 0)
            {
                status = CMS_STATUS_IO;
                break;
            }
        }
    }

    if (fclose(fp) != 0)
    {
        status = CMS_STATUS_IO;
    }

    if (status != CMS_STATUS_OK)
    {
        return status;
    }

    strncpy(db->file_path, file_path, CMS_MAX_FILE_PATH_LEN - 1);
    db->file_path[CMS_MAX_FILE_PATH_LEN - 1] = '\0';
    db->is_dirty = false;
    db->is_loaded = true;

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_insert(StudentDatabase *db, const StudentRecord *record)
{
    if (db == NULL || record == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(record->id) ||
        !cms_validate_name(record->name) ||
        !cms_validate_programme(record->programme) ||
        !cms_validate_mark(record->mark))
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    size_t existing_index = 0;
    if (cms_database_find_index(db, record->id, &existing_index))
    {
        return CMS_STATUS_DUPLICATE;
    }

    CMS_STATUS status = cms_ensure_capacity(db);
    if (status != CMS_STATUS_OK)
    {
        return status;
    }

    StudentRecord *dest = &db->records[db->count];
    dest->id = record->id;
    strncpy(dest->name, record->name, CMS_MAX_NAME_LEN);
    dest->name[CMS_MAX_NAME_LEN] = '\0';
    strncpy(dest->programme, record->programme, CMS_MAX_PROGRAMME_LEN);
    dest->programme[CMS_MAX_PROGRAMME_LEN] = '\0';
    dest->mark = record->mark;
    db->count++;

    bool prev_dirty = db->is_dirty;
    db->is_dirty = true;
    db->is_loaded = true;
    cms_set_undo_state(db, CMS_UNDO_INSERT, NULL, dest, db->count - 1, prev_dirty);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_query(const StudentDatabase *db, int student_id, StudentRecord *out_record)
{
    if (db == NULL || out_record == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(student_id))
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        return CMS_STATUS_NOT_FOUND;
    }

    size_t index = 0;
    if (!cms_database_find_index(db, student_id, &index))
    {
        return CMS_STATUS_NOT_FOUND;
    }

    *out_record = db->records[index];
    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_update(StudentDatabase *db, int student_id, const StudentRecord *new_record)
{
    if (db == NULL || new_record == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(student_id))
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_name(new_record->name) ||
        !cms_validate_programme(new_record->programme) ||
        !cms_validate_mark(new_record->mark))
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        return CMS_STATUS_NOT_FOUND;
    }

    size_t index = 0;
    if (!cms_database_find_index(db, student_id, &index))
    {
        return CMS_STATUS_NOT_FOUND;
    }

    StudentRecord previous = db->records[index];
    bool prev_dirty = db->is_dirty;

    StudentRecord *target = &db->records[index];
    target->id = student_id;
    strncpy(target->name, new_record->name, CMS_MAX_NAME_LEN);
    target->name[CMS_MAX_NAME_LEN] = '\0';
    strncpy(target->programme, new_record->programme, CMS_MAX_PROGRAMME_LEN);
    target->programme[CMS_MAX_PROGRAMME_LEN] = '\0';
    target->mark = new_record->mark;

    db->is_dirty = true;
    cms_set_undo_state(db, CMS_UNDO_UPDATE, &previous, target, index, prev_dirty);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_delete(StudentDatabase *db, int student_id)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(student_id))
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        return CMS_STATUS_NOT_FOUND;
    }

    size_t index = 0;
    if (!cms_database_find_index(db, student_id, &index))
    {
        return CMS_STATUS_NOT_FOUND;
    }

    StudentRecord removed = db->records[index];
    bool prev_dirty = db->is_dirty;

    if (index < db->count - 1)
    {
        memmove(&db->records[index],
                &db->records[index + 1],
                (db->count - index - 1) * sizeof(StudentRecord));
    }

    db->count--;
    db->is_dirty = true;
    cms_set_undo_state(db, CMS_UNDO_DELETE, &removed, NULL, index, prev_dirty);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_undo(StudentDatabase *db)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!db->undo_state.valid || db->undo_state.action == CMS_UNDO_NONE)
    {
        printf("CMS: Nothing to undo.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CMS_STATUS status = CMS_STATUS_OK;

    switch (db->undo_state.action)
    {
    case CMS_UNDO_INSERT:
    {
        size_t index = 0;
        if (!cms_database_find_index(db, db->undo_state.after.id, &index))
        {
            printf("CMS: Unable to undo insert; record not found.\n");
            cms_clear_undo_state(db);
            return CMS_STATUS_NOT_FOUND;
        }

        if (index < db->count - 1)
        {
            memmove(&db->records[index],
                    &db->records[index + 1],
                    (db->count - index - 1) * sizeof(StudentRecord));
        }
        db->count--;
        db->is_dirty = db->undo_state.prev_dirty;
        break;
    }
    case CMS_UNDO_DELETE:
    {
        status = cms_ensure_capacity(db);
        if (status != CMS_STATUS_OK)
        {
            return status;
        }

        size_t insert_index = db->undo_state.index;
        if (insert_index > db->count)
        {
            insert_index = db->count;
        }

        if (insert_index < db->count)
        {
            memmove(&db->records[insert_index + 1],
                    &db->records[insert_index],
                    (db->count - insert_index) * sizeof(StudentRecord));
        }

        db->records[insert_index] = db->undo_state.before;
        db->count++;
        db->is_dirty = db->undo_state.prev_dirty;
        break;
    }
    case CMS_UNDO_UPDATE:
    {
        size_t index = db->undo_state.index;
        if (index >= db->count || db->records[index].id != db->undo_state.before.id)
        {
            if (!cms_database_find_index(db, db->undo_state.before.id, &index))
            {
                printf("CMS: Unable to undo update; record not found.\n");
                cms_clear_undo_state(db);
                return CMS_STATUS_NOT_FOUND;
            }
        }

        db->records[index] = db->undo_state.before;
        db->is_dirty = db->undo_state.prev_dirty;
        break;
    }
    default:
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    cms_clear_undo_state(db);
    printf("CMS: Last change has been undone.\n");
    return status;
}

CMS_STATUS cms_database_show_all(const StudentDatabase *db)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0)
    {
        printf("\nNo records available.\n\n");
        return CMS_STATUS_OK;
    }

    cms_display_table(db);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_show_record(const StudentRecord *record)
{
    /* TODO: Implement single record display */
    if (record == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Print record details */
    printf("ID: %d, Name: %s, Programme: %s, Mark: %.2f\n",
           record->id, record->name, record->programme, record->mark);

    return CMS_STATUS_OK;
}

CMS_STATUS cms_database_show_sorted(const StudentDatabase *db, CmsSortKey sort_key, CmsSortOrder sort_order)
{
    /* Display sorted records without modifying the original database */
    sort_key = sort_key;
    sort_order = sort_order;
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
        // return CMS_STATUS_NOT_FOUND;
    }

    if (db->records == NULL || db->count == 0)
    {
        printf("No records to display.\n");
        return CMS_STATUS_OK;
    }

    /* Create a copy of the records to sort */
    StudentRecord *sorted_records = (StudentRecord *)malloc(db->count * sizeof(StudentRecord));
    if (sorted_records == NULL)
    {
        return CMS_STATUS_ERROR;
    }

    /* Copy records */
    memcpy(sorted_records, db->records, db->count * sizeof(StudentRecord));

    StudentDatabase *sortable_db = malloc(sizeof(StudentDatabase));
    if (sortable_db == NULL)
    {
        free(sorted_records);
        return CMS_STATUS_ERROR;
    }
    sortable_db->records = sorted_records;
    sortable_db->count = db->count;

    // Updated sorting logic to use the cms_sort functions and sortable_db
    switch(sort_key){
        case CMS_SORT_KEY_ID:
            cms_sort_by_id(sortable_db, sort_order);
            break;
        case CMS_SORT_KEY_MARK:
            cms_sort_by_mark(sortable_db, sort_order);
            break;
        case CMS_SORT_KEY_NAME:
            cms_sort_by_name(sortable_db, sort_order);
            break;
        case CMS_SORT_KEY_PROGRAMME:
            cms_sort_by_prog(sortable_db, sort_order);
            break;
        default:
            cms_sort_by_id(sortable_db, sort_order);
            break;
    }

    /* Legacy Display header, replaced with cms_display_table for standardisation */
    // printf("\nTable Name: StudentRecords\n");
    // printf("%-12s %-20s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    // printf("%-12s %-20s %-30s %-10s\n", "----", "----", "---------", "----");

    // /* Display sorted records */
    // for (size_t i = 0; i < db->count; i++)
    // {
    //     printf("%-12d %-20s %-30s %-10.1f\n",
    //            sorted_records[i].id,
    //            sorted_records[i].name,
    //            sorted_records[i].programme,
    //            sorted_records[i].mark);
    // }
    // printf("\n");
    cms_display_table(sortable_db);

    /* Free the copied records */
    free(sorted_records);
    free(sortable_db);

    return CMS_STATUS_OK;
}
