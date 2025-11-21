#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "summary.h"
#include "database.h"

/* Comparison functions for qsort */
static int compare_by_id_asc(const void *a, const void *b) {
    /* TODO: Implement ID ascending comparison */
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;
    return rec_a->id - rec_b->id;
}

static int compare_by_id_desc(const void *a, const void *b) {
    /* TODO: Implement ID descending comparison */
    return compare_by_id_asc(b, a);
}

static int compare_by_mark_asc(const void *a, const void *b) {
    /* TODO: Implement mark ascending comparison */
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;
    
    if (rec_a->mark < rec_b->mark) return -1;
    if (rec_a->mark > rec_b->mark) return 1;
    return 0;
}

static int compare_by_mark_desc(const void *a, const void *b) {
    /* TODO: Implement mark descending comparison */
    return compare_by_mark_asc(b, a);
}

static int compare_by_name_asc(const void *a, const void *b) {
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;

    return strcmp(rec_a->name, rec_b->name);
}

static int compare_by_name_desc(const void *a, const void *b) {
    return compare_by_name_asc(b, a);
}

static int compare_by_programme_asc(const void *a, const void *b) {
    const StudentRecord *rec_a = (const StudentRecord *)a;
    const StudentRecord *rec_b = (const StudentRecord *)b;

    return strcmp(rec_a->programme, rec_b->programme);
}

static int compare_by_programme_desc(const void *a, const void *b) {
    return compare_by_programme_asc(b, a);
}

CMS_STATUS cms_sort_by_id(StudentDatabase *db, SortOrder order) {
    /* TODO: Implement sorting by ID */
    if (db == NULL || db->records == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING) {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_asc);
    } else {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_id_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_sort_by_mark(StudentDatabase *db, SortOrder order) {
    /* TODO: Implement sorting by mark */
    if (db == NULL || db->records == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (order == SORT_ASCENDING) {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_asc);
    } else {
        qsort(db->records, db->count, sizeof(StudentRecord), compare_by_mark_desc);
    }

    return CMS_STATUS_OK;
}

CMS_STATUS cms_calculate_summary(const StudentDatabase *db, SummaryStats *stats) {
    if (db == NULL || stats == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (db->records == NULL || db->count == 0) {
        return CMS_STATUS_NOT_FOUND;
    }

    stats->count = db->count;
    float total = 0.0f;

    stats->highest = db->records[0].mark;
    stats->lowest = db->records[0].mark;
    stats->highest_id = db->records[0].id;
    stats->lowest_id = db->records[0].id;

    for (size_t i = 0; i < db->count; ++i) {
        const StudentRecord *record = &db->records[i];
        total += record->mark;

        if (record->mark > stats->highest) {
            stats->highest = record->mark;
            stats->highest_id = record->id;
        }

        if (record->mark < stats->lowest) {
            stats->lowest = record->mark;
            stats->lowest_id = record->id;
        }
    }

    stats->average = total / (float)stats->count;

    return CMS_STATUS_OK;
}

CMS_STATUS cms_display_summary(const StudentDatabase *db) {
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    SummaryStats stats;
    CMS_STATUS status = cms_calculate_summary(db, &stats);
    
    if (status == CMS_STATUS_NOT_FOUND) {
        printf("No record found for ID %d.\n", student_id);
        return status;
    } else if (status != CMS_STATUS_OK) {
        cms_print_status(status);
        return status;
    }

    printf("Current record: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n",
           current_record.id, current_record.name,
           current_record.programme, current_record.mark);

    StudentRecord updated_record = current_record;
    char input_buffer[CMS_MAX_PROGRAMME_LEN + 1];

    /* Update name */
    while (1) {
        if (!cms_read_string("Enter name (or press Enter to keep current): ",
                             input_buffer, sizeof(input_buffer))) {
            return CMS_STATUS_ERROR;
        }

        if (input_buffer[0] == '\0') {
            break;
        }

        if (!cms_validate_name(input_buffer)) {
            printf("Invalid name. Please try again.\n");
            continue;
        }

        strncpy(updated_record.name, input_buffer, CMS_MAX_NAME_LEN);
        updated_record.name[CMS_MAX_NAME_LEN] = '\0';
        break;
    }

    /* Update programme */
    while (1) {
        if (!cms_read_string("Enter programme (or press Enter to keep current): ",
                             input_buffer, sizeof(input_buffer))) {
            return CMS_STATUS_ERROR;
        }

        if (input_buffer[0] == '\0') {
            break;
        }

        if (!cms_validate_programme(input_buffer)) {
            printf("Invalid programme. Please try again.\n");
            continue;
        }

        strncpy(updated_record.programme, input_buffer, CMS_MAX_PROGRAMME_LEN);
        updated_record.programme[CMS_MAX_PROGRAMME_LEN] = '\0';
        break;
    }

    /* Update mark */
    char mark_buffer[32];
    while (1) {
        printf("Enter mark (or press Enter to keep current): ");
        if (!cms_read_line(mark_buffer, sizeof(mark_buffer))) {
            return CMS_STATUS_ERROR;
        }

        if (mark_buffer[0] == '\0') {
            break;
        }

        char *end_ptr = NULL;
        float new_mark = strtof(mark_buffer, &end_ptr);

        if (end_ptr == mark_buffer || *end_ptr != '\0' || !cms_validate_mark(new_mark)) {
            printf("Invalid mark. Please enter a value between %.2f and %.2f.\n",
                   CMS_MIN_MARK, CMS_MAX_MARK);
            continue;
        }

        updated_record.mark = new_mark;
        break;
    }

    status = cms_database_update(db, student_id, &updated_record);

    if (status == CMS_STATUS_OK) {
        printf("Record updated successfully.\n");
    } else {
        cms_print_status(status);
    }

    return status;
}

CMS_STATUS cmd_delete(StudentDatabase *db, int student_id) {
    /* TODO: Implement DELETE command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    return cms_database_delete(db, student_id);
}

CMS_STATUS cmd_save(StudentDatabase *db, const char *filename) {
    /* TODO: Implement SAVE command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    const char *save_path = (filename != NULL) ? filename : db->file_path;
    return cms_database_save(db, save_path);
}

CMS_STATUS cmd_help(void) {
    /* TODO: Implement HELP command */
    printf("\nAvailable Commands:\n");
    printf("  OPEN <filename>              - Load a database file\n");
    printf("  SHOW [ALL|SUMMARY|ID|MARK|NAME|PROGRAMME] [ASC|DESC] - Display records (defaults to ID ASC)\n");
    printf("  INSERT                       - Add a new student record\n");
    printf("  QUERY <student_id>          - Find a specific record\n");
    printf("  UPDATE <student_id>         - Modify an existing record\n");
    printf("  DELETE <student_id>         - Remove a student record\n");
    printf("  SAVE [filename]             - Save changes to file\n");
    printf("  HELP                        - Display this help\n");
    printf("  EXIT or QUIT                - Exit the application\n\n");

    return CMS_STATUS_OK;
}

void cms_command_loop(StudentDatabase *db) {
    char input[CMS_MAX_COMMAND_LEN];

    while (1) {
        printf("CMS> ");

        if (!cms_read_line(input, CMS_MAX_COMMAND_LEN)) {
            printf("\n");
            break;
        }

        cms_trim(input);
        if (input[0] == '\0') {
            continue;
        }

        if (cms_string_equals_ignore_case(input, "EXIT") ||
            cms_string_equals_ignore_case(input, "QUIT")) {
            printf("Exiting CMS.\n");
            break;
        }

        CMS_STATUS status = cms_parse_command(input, db);
        if (status != CMS_STATUS_OK) {
            cms_print_status(status);
        }
    }
}

CMS_STATUS cms_parse_command(const char *input, StudentDatabase *db) {
    if (input == NULL || db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    cms_trim(buffer);

    if (buffer[0] == '\0') {
        return CMS_STATUS_OK;
    }

    char *args = NULL;
    char *command = buffer;
    char *split = buffer;
    while (*split && !isspace((unsigned char)*split)) {
        split++;
    }
    if (*split != '\0') {
        *split = '\0';
        args = split + 1;
        while (*args && isspace((unsigned char)*args)) {
            args++;
        }
        if (args[0] == '\0') {
            args = NULL;
        }
    }

    cms_string_to_upper(command);

    if (strcmp(command, "HELP") == 0) {
        if (args != NULL) {
            printf("HELP does not take any arguments. Ignoring extra input.\n");
        }
        return cmd_help();
    }

    if (strcmp(command, "OPEN") == 0) {
        const char *path = args;
        return cmd_open(db, path);
    }

    if (strcmp(command, "SHOW") == 0) {
        if (args == NULL) {
            /* Default to showing ID ascending when no arguments are given */
            return cmd_show(db, NULL, NULL);
        }

        char *option = args;
        char *order = NULL;
        char *cursor = option;
        while (*cursor && !isspace((unsigned char)*cursor)) {
            cursor++;
        }
        if (*cursor != '\0') {
            *cursor = '\0';
            cursor++;
            while (*cursor && isspace((unsigned char)*cursor)) {
                cursor++;
            }
            if (*cursor != '\0') {
                order = cursor;
                char *order_end = order;
                while (*order_end && !isspace((unsigned char)*order_end)) {
                    order_end++;
                }
                if (*order_end != '\0') {
                    *order_end = '\0';
                    order_end++;
                    while (*order_end && isspace((unsigned char)*order_end)) {
                        order_end++;
                    }
                    if (*order_end != '\0') {
                        printf("Usage: SHOW [ALL|SUMMARY|ID|MARK|NAME|PROGRAMME] [ASC|DESC]\n");
                        return CMS_STATUS_OK;
                    }
                }
            }
        }

        return cmd_show(db, option, order);
    }

    if (strcmp(command, "INSERT") == 0) {
        if (args != NULL) {
            printf("Usage: INSERT\n");
            return CMS_STATUS_OK;
        }
        return cmd_insert(db);
    }

    if (strcmp(command, "QUERY") == 0) {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id)) {
            printf("Usage: QUERY <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_query(db, student_id);
    }

    if (strcmp(command, "UPDATE") == 0) {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id)) {
            printf("Usage: UPDATE <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_update(db, student_id);
    }

    if (strcmp(command, "DELETE") == 0) {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id)) {
            printf("Usage: DELETE <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_delete(db, student_id);
    }

    if (strcmp(command, "SAVE") == 0) {
        const char *save_path = args;
        return cmd_save(db, save_path);
    }

    if (strcmp(command, "EXIT") == 0 || strcmp(command, "QUIT") == 0) {
        printf("Type EXIT or QUIT directly at the prompt to leave the program.\n");
        return CMS_STATUS_OK;
    }

    printf("Unknown command. Type HELP to see the list of commands.\n");
    return CMS_STATUS_OK;
}

