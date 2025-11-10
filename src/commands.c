#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/commands.h"
#include "../include/database.h"
#include "../include/summary.h"
#include "../include/utils.h"
#include "../include/config.h"

CMS_STATUS cmd_open(StudentDatabase *db, const char *filename) {
    /* TODO: Implement OPEN command */
    if (db == NULL || filename == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    return cms_database_load(db, filename);
}

CMS_STATUS cmd_show(const StudentDatabase *db, const char *option, const char *order) {
    /* TODO: Implement SHOW command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Handle ALL, SUMMARY, ID, MARK options */
    /* TODO: Handle ASC, DESC sorting */
    return CMS_STATUS_NOT_IMPLEMENTED;
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

