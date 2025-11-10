#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/commands.h"
#include "../include/database.h"
#include "../include/summary.h"
#include "../include/utils.h"
#include "../include/config.h"

/**
 * Opens a student database file.
 * @param db Pointer to the StudentDatabase structure to load data into.
 * @param filename Path to the database file (uses CMS_DEFAULT_DATABASE_FILE if NULL or empty).
 * @return CMS_STATUS_OK on success, CMS_STATUS_INVALID_ARGUMENT or error code otherwise.
 */
CMS_STATUS cmd_open(StudentDatabase *db, const char *filename) {
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char path_buffer[CMS_MAX_FILE_PATH_LEN];
    if (filename == NULL || filename[0] == '\0') {
        filename = CMS_DEFAULT_DATABASE_FILE;
    }

    strncpy(path_buffer, filename, sizeof(path_buffer) - 1);
    path_buffer[sizeof(path_buffer) - 1] = '\0';
    cms_trim(path_buffer);

    if (path_buffer[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    return cms_database_load(db, path_buffer);
}

/**
 * Displays student records or summary information.
 * @param db Pointer to the StudentDatabase structure to read from.
 * @param option Display mode: "ALL", "SUMMARY", "ID", or "MARK".
 * @param order Sort order for "ID" or "MARK": "ASC" or "DESC".
 * @return CMS_STATUS_OK on success, CMS_STATUS_INVALID_ARGUMENT or error code otherwise.
 */
CMS_STATUS cmd_show(const StudentDatabase *db, const char *option, const char *order) {
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (option == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char opt_buf[32];
    char ord_buf[16];

    strncpy(opt_buf, option, sizeof(opt_buf) - 1);
    opt_buf[sizeof(opt_buf) - 1] = '\0';
    cms_trim(opt_buf);

    if (order) {
        strncpy(ord_buf, order, sizeof(ord_buf) - 1);
        ord_buf[sizeof(ord_buf) - 1] = '\0';
        cms_trim(ord_buf);
    } else {
        ord_buf[0] = '\0';
    }

    if (cms_string_equals_ignore_case(opt_buf, "SUMMARY")) {
        return cms_show_summary(db);
    }

    if (cms_string_equals_ignore_case(opt_buf, "ALL")) {
        if (ord_buf[0] != '\0') {
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        return cms_show_all(db);
    }

    CmsSortKey sort_key;
    if (cms_string_equals_ignore_case(opt_buf, "ID")) {
        sort_key = CMS_SORT_KEY_ID;
    } else if (cms_string_equals_ignore_case(opt_buf, "MARK")) {
        sort_key = CMS_SORT_KEY_MARK;
    } else {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CmsSortOrder sort_order = CMS_SORT_ASC;
    if (ord_buf[0] != '\0') {
        if (cms_string_equals_ignore_case(ord_buf, "ASC")) {
            sort_order = CMS_SORT_ASC;
        } else if (cms_string_equals_ignore_case(ord_buf, "DESC")) {
            sort_order = CMS_SORT_DESC;
        } else {
            return CMS_STATUS_INVALID_ARGUMENT;
        }
    }

    return cms_show_all_sorted(db, sort_key, sort_order);
}

CMS_STATUS cmd_insert(StudentDatabase *db) {
    /* TODO: Implement INSERT command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Prompt for student details */
    /* TODO: Validate input */
    /* TODO: Insert record */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CMS_STATUS cmd_query(const StudentDatabase *db, int student_id) {
    /* TODO: Implement QUERY command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord record;
    CMS_STATUS status = cms_database_query(db, student_id, &record);
    
    if (status == CMS_STATUS_OK) {
        cms_database_show_record(&record);
    }

    return status;
}

CMS_STATUS cmd_update(StudentDatabase *db, int student_id) {
    /* TODO: Implement UPDATE command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Query existing record */
    /* TODO: Prompt for new values */
    /* TODO: Update record */
    return CMS_STATUS_NOT_IMPLEMENTED;
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
    printf("  SHOW [ALL|SUMMARY|ID|MARK] [ASC|DESC] - Display records\n");
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
    /* TODO: Implement main command loop */
    char input[CMS_MAX_COMMAND_LEN];

    while (1) {
        printf("CMS> ");
        
        if (!cms_read_line(input, CMS_MAX_COMMAND_LEN)) {
            break;
        }

        /* TODO: Parse and execute command */
        CMS_STATUS status = cms_parse_command(input, db);
        
        if (status == CMS_STATUS_OK) {
            /* Command executed successfully */
        } else if (status == CMS_STATUS_NOT_IMPLEMENTED) {
            printf("Command not yet implemented\n");
        }
    }
}

CMS_STATUS cms_parse_command(const char *input, StudentDatabase *db) {
    /* TODO: Implement command parsing */
    if (input == NULL || db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Tokenize input */
    /* TODO: Match command keywords */
    /* TODO: Call appropriate handler */

    return CMS_STATUS_NOT_IMPLEMENTED;
}

