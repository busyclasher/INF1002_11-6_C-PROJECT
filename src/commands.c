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

    char key_str[32] = "";
    char order_str[32] = "";

    if (sort_key == CMS_SORT_KEY_ID)
    {
        strcpy(key_str, "ID");
    }
    else if (sort_key == CMS_SORT_KEY_MARK)
    {
        strcpy(key_str, "MARK");
    }

    if (sort_order == CMS_SORT_ASC)
    {
        strcpy(order_str, "ASC");
    }
    else if (sort_order == CMS_SORT_DESC)
    {
        strcpy(order_str, "DESC");
    }
    return cms_database_show_sorted(db, key_str, order_str);
}

CMS_STATUS cmd_insert(StudentDatabase *db, const char *params)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!db->is_loaded)
    {
        printf("CMS: No database is currently opened.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord record;
    memset(&record, 0, sizeof(record));

    bool has_id = false;
    bool has_name = false;
    bool has_programme = false;
    bool has_mark = false;

    char buffer[CMS_MAX_COMMAND_LEN];
    if (params != NULL)
    {
        strncpy(buffer, params, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        cms_trim_string(buffer);
    }
    else
    {
        buffer[0] = '\0';
    }

    /* Parse inline parameters if provided (e.g. "ID=2401234 NAME=MICHELLE LEE ...") */
    if (buffer[0] != '\0')
    {
        char *ptr = buffer;
        while (*ptr != '\0')
        {
            /* Skip leading spaces */
            while (*ptr == ' ')
            {
                ptr++;
            }
            if (*ptr == '\0')
            {
                break;
            }

            char *eq = strchr(ptr, '=');
            if (eq == NULL)
            {
                break; /* malformed, stop parsing further */
            }

            *eq = '\0';
            char *key_start = ptr;
            char *value_start = eq + 1;

            /* Find end of value: before next KEY= token or end-of-string */
            char *value_end = value_start;
            while (*value_end != '\0')
            {
                if (*value_end == ' ')
                {
                    char *next = value_end + 1;
                    if (strncmp(next, "ID=", 3) == 0 ||
                        strncmp(next, "NAME=", 5) == 0 ||
                        strncmp(next, "PROGRAMME=", 10) == 0 ||
                        strncmp(next, "MARK=", 5) == 0)
                    {
                        break;
                    }
                }
                value_end++;
            }

            char saved = *value_end;
            *value_end = '\0';

            cms_trim_string(key_start);
            cms_trim_string(value_start);

            if (strcmp(key_start, "ID") == 0)
            {
                record.id = atoi(value_start);
                has_id = true;
            }
            else if (strcmp(key_start, "NAME") == 0)
            {
                strncpy(record.name, value_start, sizeof(record.name) - 1);
                record.name[sizeof(record.name) - 1] = '\0';
                has_name = true;
            }
            else if (strcmp(key_start, "PROGRAMME") == 0)
            {
                strncpy(record.programme, value_start, sizeof(record.programme) - 1);
                record.programme[sizeof(record.programme) - 1] = '\0';
                has_programme = true;
            }
            else if (strcmp(key_start, "MARK") == 0)
            {
                record.mark = (float)atof(value_start);
                has_mark = true;
            }

            *value_end = saved;
            ptr = value_end;
        }
    }

    /* If ID not provided as a parameter, prompt for it */
    if (!has_id)
    {
        int id = 0;
        if (!cms_read_int("Enter student ID: ", &id))
        {
            printf("CMS: Invalid input for ID.\n");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        record.id = id;
        has_id = true;
    }

    /* Validate ID */
    if (!cms_validate_student_id(record.id))
    {
        printf("CMS: Invalid student ID.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Check for duplicate ID before asking for other fields */
    for (size_t i = 0; i < db->count; ++i)
    {
        if (db->records[i].id == record.id)
        {
            printf("CMS: The record with ID=%d already exists.\n", record.id);
            return CMS_STATUS_DUPLICATE;
        }
    }

    /* Prompt for any missing fields (interactive mode) */
    if (!has_name)
    {
        if (!cms_read_string("Enter name: ", record.name, sizeof(record.name)))
        {
            printf("CMS: Invalid input for name.\n");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        cms_trim_string(record.name);
    }

    if (!has_programme)
    {
        if (!cms_read_string("Enter programme: ", record.programme, sizeof(record.programme)))
        {
            printf("CMS: Invalid input for programme.\n");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        cms_trim_string(record.programme);
    }

    if (!has_mark)
    {
        float mark = 0.0f;
        if (!cms_read_float("Enter mark (0-100): ", &mark))
        {
            printf("CMS: Invalid input for mark.\n");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        record.mark = mark;
    }

    /* Validate remaining fields */
    if (!cms_validate_name(record.name) ||
        !cms_validate_programme(record.programme) ||
        !cms_validate_mark(record.mark))
    {
        printf("CMS: Invalid data for new record.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CMS_STATUS status = cms_database_insert(db, &record);
    if (status == CMS_STATUS_OK)
    {
        printf("CMS: A new record with ID=%d is successfully inserted.\n", record.id);
    }
    else if (status == CMS_STATUS_DUPLICATE)
    {
        printf("CMS: The record with ID=%d already exists.\n", record.id);
    }
    else
    {
        printf("CMS: Failed to insert new record.\n");
    }

    return status;
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

CMS_STATUS cmd_update(StudentDatabase *db, int student_id)
{
    /* TODO: Implement UPDATE command */
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Query existing record */
    /* TODO: Prompt for new values */
    /* TODO: Update record */
    return CMS_STATUS_NOT_IMPLEMENTED;
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
