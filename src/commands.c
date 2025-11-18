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
CMS_STATUS cmd_open(StudentDatabase *db, const char *filename)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char path_buffer[CMS_MAX_FILE_PATH_LEN];
    if (filename == NULL || filename[0] == '\0')
    {
        filename = CMS_DEFAULT_DATABASE_FILE;
    }

    strncpy(path_buffer, filename, sizeof(path_buffer) - 1);
    path_buffer[sizeof(path_buffer) - 1] = '\0';
    cms_trim_string(path_buffer);

    if (path_buffer[0] == '\0')
    {
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
CMS_STATUS cmd_show(const StudentDatabase *db, const char *option, const char *order)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (option == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char opt_buf[32];
    char ord_buf[16];

    strncpy(opt_buf, option, sizeof(opt_buf) - 1);
    opt_buf[sizeof(opt_buf) - 1] = '\0';
    cms_trim_string(opt_buf);

    if (order)
    {
        strncpy(ord_buf, order, sizeof(ord_buf) - 1);
        ord_buf[sizeof(ord_buf) - 1] = '\0';
        cms_trim_string(ord_buf);
    }
    else
    {
        ord_buf[0] = '\0';
    }

    cms_string_to_upper(opt_buf);

    if (strcmp(opt_buf, "SUMMARY") == 0)
    {
        return cms_display_summary(db);
    }

    if (strcmp(opt_buf, "ALL") == 0)
    {
        return cms_database_show_all(db);
    }

    CmsSortKey sort_key;
    if (strcmp(opt_buf, "ID") == 0)
    {
        sort_key = CMS_SORT_KEY_ID;
    }
    else if (strcmp(opt_buf, "MARK") == 0)
    {
        sort_key = CMS_SORT_KEY_MARK;
    }
    else
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CmsSortOrder sort_order = CMS_SORT_ASC;
    if (ord_buf[0] != '\0')
    {
        if (strcmp(ord_buf, "ASC") == 0)
        {
            sort_order = CMS_SORT_ASC;
        }
        else if (strcmp(ord_buf, "DESC") == 0)
        {
            sort_order = CMS_SORT_DESC;
        }
        else
        {
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

CMS_STATUS cmd_query(const StudentDatabase *db, int student_id)
{
    /* TODO: Implement QUERY command */
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord record;
    CMS_STATUS status = cms_database_query(db, student_id, &record);

    if (status == CMS_STATUS_OK)
    {
        cms_database_show_record(&record);
    }

    return status;
}

CMS_STATUS cmd_update(StudentDatabase *db, int student_id) {
    /* TODO: Implement UPDATE command */
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(student_id)) {
        printf("Invalid student ID.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord current_record;
    CMS_STATUS status = cms_database_query(db, student_id, &current_record);

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

CMS_STATUS cmd_delete(StudentDatabase *db, int student_id)
{
    /* TODO: Implement DELETE command */
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    // StudentRecord *recordPointer = NULL;

    // for (size_t i = 0; i < db->count; i++) {
    //     StudentRecord *record = &db -> records[i];
    //     if (record -> id == student_id) {
    //         recordPointer = &db->records[i];
    //         break;
    //     }
    // }

    // if (recordPointer == NULL) {
    //     printf("Record not found\n");
    //     return CMS_STATUS_NOT_FOUND;
    // } else {
    //     printf(" %c", (char)recordPointer);
    // }

    return cms_database_delete(db, student_id);
}

CMS_STATUS cmd_save(StudentDatabase *db, const char *filename)
{
    /* TODO: Implement SAVE command */
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    const char *save_path = (filename != NULL) ? filename : db->file_path;
    return cms_database_save(db, save_path);
}

CMS_STATUS cmd_help(void)
{
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

void cms_command_loop(StudentDatabase *db)
{
    /* TODO: Implement main command loop */
    char input[CMS_MAX_COMMAND_LEN];

    while (1)
    {
        printf("CMS> ");

        if (!cms_read_line(input, CMS_MAX_COMMAND_LEN))
        {
            break;
        }

        /* TODO: Parse and execute command */
        CMS_STATUS status = cms_parse_command(input, db);

        if (status == CMS_STATUS_OK)
        {
            /* Command executed successfully */
        }
        else if (status == CMS_STATUS_NOT_IMPLEMENTED)
        {
            printf("Command not yet implemented\n");
        }
    }
}

CMS_STATUS cms_parse_command(const char *input, StudentDatabase *db)
{
    /* TODO: Implement command parsing */
    if (input == NULL || db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Tokenize input */

    // Auto capitalise input
    // cms_string_to_upper((char *)input);
    cms_trim_string((char *)input);

    /* TODO: Match command keywords */
    if (strncmp(input, "OPEN", 4) == 0)
    {
        char assumedFileLocation[255] = "./";
        strcat(assumedFileLocation, input + 5);
        // Skip space if present
        if (assumedFileLocation[0] == ' ')
            memmove(assumedFileLocation, assumedFileLocation + 1, strlen(assumedFileLocation));
        cms_database_load(db, assumedFileLocation);
        return CMS_STATUS_OK;
    }
    else if (strncmp(input, "SHOW", 4) == 0)
    {
        return cmd_show(db, "ALL", "ASC");
    }
    else if (strncmp(input, "INSERT", 6) == 0)
    {
        const char *params = input + 6;
        if (*params == ' ')
            params++; // skip space
        return cmd_insert(db, params);
    }
    else if (strncmp(input, "HELP", 4) == 0)
    {
        return cmd_help();
    }
    /* TODO: Call appropriate handler */

    return CMS_STATUS_NOT_IMPLEMENTED;
}
