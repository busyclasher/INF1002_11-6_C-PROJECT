#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    else if (strcmp(opt_buf, "NAME") == 0)
    {
        sort_key = CMS_SORT_KEY_NAME;
    }
    else if (strcmp(opt_buf, "PROGRAMME") == 0)
    {
        sort_key = CMS_SORT_KEY_PROGRAMME;
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

    return cms_database_show_sorted(db, sort_key, sort_order);
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
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!cms_validate_student_id(student_id))
    {
        printf("Invalid student ID.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord current_record;
    CMS_STATUS status = cms_database_query(db, student_id, &current_record);

    if (status == CMS_STATUS_NOT_FOUND)
    {
        printf("No record found for ID %d.\n", student_id);
        return status;
    }
    else if (status != CMS_STATUS_OK)
    {
        cms_print_status(status);
        return status;
    }

    printf("Current record: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n",
           current_record.id, current_record.name,
           current_record.programme, current_record.mark);

    StudentRecord updated_record = current_record;
    char input_buffer[CMS_MAX_PROGRAMME_LEN + 1];

    /* Update name */
    while (1)
    {
        if (!cms_read_string("Enter name (or press Enter to keep current): ",
                             input_buffer, sizeof(input_buffer)))
        {
            return CMS_STATUS_ERROR;
        }

        if (input_buffer[0] == '\0')
        {
            break;
        }

        if (!cms_validate_name(input_buffer))
        {
            printf("Invalid name. Please try again.\n");
            continue;
        }

        strncpy(updated_record.name, input_buffer, CMS_MAX_NAME_LEN);
        updated_record.name[CMS_MAX_NAME_LEN] = '\0';
        break;
    }

    /* Update programme */
    while (1)
    {
        if (!cms_read_string("Enter programme (or press Enter to keep current): ",
                             input_buffer, sizeof(input_buffer)))
        {
            return CMS_STATUS_ERROR;
        }

        if (input_buffer[0] == '\0')
        {
            break;
        }

        if (!cms_validate_programme(input_buffer))
        {
            printf("Invalid programme. Please try again.\n");
            continue;
        }

        strncpy(updated_record.programme, input_buffer, CMS_MAX_PROGRAMME_LEN);
        updated_record.programme[CMS_MAX_PROGRAMME_LEN] = '\0';
        break;
    }

    /* Update mark */
    char mark_buffer[32];
    while (1)
    {
        printf("Enter mark (or press Enter to keep current): ");
        if (!cms_read_line(mark_buffer, sizeof(mark_buffer)))
        {
            return CMS_STATUS_ERROR;
        }

        if (mark_buffer[0] == '\0')
        {
            break;
        }

        char *end_ptr = NULL;
        float new_mark = strtof(mark_buffer, &end_ptr);

        if (end_ptr == mark_buffer || *end_ptr != '\0' || !cms_validate_mark(new_mark))
        {
            printf("Invalid mark. Please enter a value between %.2f and %.2f.\n",
                   CMS_MIN_MARK, CMS_MAX_MARK);
            continue;
        }

        updated_record.mark = new_mark;
        break;
    }

    status = cms_database_update(db, student_id, &updated_record);

    if (status == CMS_STATUS_OK)
    {
        printf("Record updated successfully.\n");
    }
    else
    {
        cms_print_status(status);
    }

    return status;
}

CMS_STATUS cmd_delete(StudentDatabase *db, int student_id)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Ensure a database is loaded */
    if (!db->is_loaded)
    {
        printf("CMS> No database is currently opened.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Validate student ID */
    if (!cms_validate_student_id(student_id))
    {
        printf("CMS: Invalid student ID.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Check if record exists first */
    StudentRecord tmp_record;
    CMS_STATUS status = cms_database_query(db, student_id, &tmp_record);

    if (status == CMS_STATUS_NOT_FOUND)
    {
        printf("CMS: The record with ID=%d does not exist.\n", student_id);
        return status;
    }
    else if (status != CMS_STATUS_OK)
    {
        cms_print_status(status);
        return status;
    }

    /* Confirmation loop */
    char input_buffer[32];
    while (1)
    {
        printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to confirm or type \"N\" to cancel.\n",
               student_id);

        if (!cms_read_line(input_buffer, sizeof(input_buffer)))
        {
            /* Treat read failure as cancellation */
            printf("CMS: The deletion is cancelled.\n");
            return CMS_STATUS_ERROR;
        }

        cms_trim_string(input_buffer);
        cms_string_to_upper(input_buffer);

        if (strcmp(input_buffer, "Y") == 0)
        {
            status = cms_database_delete(db, student_id);
            if (status == CMS_STATUS_OK)
            {
                printf("CMS: The record with ID=%d is successfully deleted.\n", student_id);
            }
            else
            {
                cms_print_status(status);
            }
            return status;
        }
        else if (strcmp(input_buffer, "N") == 0)
        {
            printf("CMS: The deletion is cancelled.\n");
            return CMS_STATUS_OK;
        }
        else
        {
            printf("CMS: Invalid input. Please type \"Y\" to confirm or \"N\" to cancel.\n");
        }
    }
}

CMS_STATUS cmd_filter(const StudentDatabase *db, const char *programme)
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

    if (programme == NULL || programme[0] == '\0')
    {
        printf("Usage: FILTER <programme>\n");
        return CMS_STATUS_OK;
    }

    /* Accept either:
       - FILTER <programme>
       - FILTER PROGRAMME <programme> (ignore the PROGRAMME keyword if present) */
    char prog_buf[CMS_MAX_COMMAND_LEN];
    strncpy(prog_buf, programme, sizeof(prog_buf) - 1);
    prog_buf[sizeof(prog_buf) - 1] = '\0';
    cms_trim_string(prog_buf);

    if (prog_buf[0] == '\0')
    {
        printf("Usage: FILTER <programme>\n");
        return CMS_STATUS_OK;
    }

    /* If the args start with the keyword PROGRAMME, skip it */
    const char *keyword = "PROGRAMME";
    size_t keyword_len = strlen(keyword);
    if (cms_string_equals_ignore_case(prog_buf, keyword))
    {
        printf("Usage: FILTER <programme>\n");
        return CMS_STATUS_OK;
    }
    else
    {
        /* Case-insensitive prefix check for "PROGRAMME " */
        size_t buf_len = strlen(prog_buf);
        if (buf_len > keyword_len)
        {
            bool prefix_match = true;
            for (size_t i = 0; i < keyword_len; ++i)
            {
                if (tolower((unsigned char)prog_buf[i]) != tolower((unsigned char)keyword[i]))
                {
                    prefix_match = false;
                    break;
                }
            }

            if (prefix_match && isspace((unsigned char)prog_buf[keyword_len]))
            {
                char *after_kw = prog_buf + keyword_len;
                while (*after_kw && isspace((unsigned char)*after_kw))
                {
                    after_kw++;
                }
                memmove(prog_buf, after_kw, strlen(after_kw) + 1);
            }
        }
    }

    if (prog_buf[0] == '\0')
    {
        printf("Usage: FILTER <programme>\n");
        return CMS_STATUS_OK;
    }

    if (db->records == NULL || db->count == 0)
    {
        printf("\nNo records available.\n\n");
        return CMS_STATUS_OK;
    }

    printf("\n%-8s  %-32s  %-20s  %-6s\n", "ID", "Name", "Programme", "Mark");
    printf("-------------------------------------------------------------------------------\n");

    size_t matches = 0;
    for (size_t i = 0; i < db->count; ++i)
    {
        const StudentRecord *record = &db->records[i];
        if (cms_string_equals_ignore_case(record->programme, prog_buf))
        {
            printf("%-8d  %-32s  %-20s  %6.2f\n",
                   record->id,
                   record->name,
                   record->programme,
                   record->mark);
            matches++;
        }
    }

    if (matches == 0)
    {
        printf("No records found for programme \"%s\".\n", prog_buf);
    }
    printf("-------------------------------------------------------------------------------\n\n");

    return CMS_STATUS_OK;
}

CMS_STATUS cmd_save(StudentDatabase *db, const char *filename)
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

    const char *save_path = (filename != NULL && filename[0] != '\0') ? filename : db->file_path;
    CMS_STATUS status = cms_database_save(db, save_path);
    if (status == CMS_STATUS_OK)
    {
        // Extract basename for cleaner display
        const char *display_name = save_path;
        char *slash = strrchr(save_path, '\\');
        if (slash == NULL)
        {
            slash = strrchr(save_path, '/');
        }
        if (slash != NULL)
        {
            display_name = slash + 1; // Use basename after last path separator
        }
        printf("CMS: The database file \"%s\" is successfully saved.\n", display_name);
    }
    return status;
}

CMS_STATUS cmd_undo(StudentDatabase *db)
{
    if (db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if (!db->is_loaded && db->count == 0)
    {
        printf("CMS: No database is currently opened.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    return cms_database_undo(db);
}

CMS_STATUS cmd_help(void)
{
    /* TODO: Implement HELP command */
    printf("\nAvailable Commands:\n");
    printf("  OPEN <filename>              - Load a database file\n");
    printf("  SHOW [ALL|SUMMARY|ID|MARK|NAME|PROGRAMME] [ASC|DESC] - Display records (defaults to ID ASC)\n");
    printf("  INSERT                       - Add a new student record\n");
    printf("  QUERY <student_id>          - Find a specific record\n");
    printf("  UPDATE <student_id>         - Modify an existing record\n");
    printf("  DELETE <student_id>         - Remove a student record\n");
    printf("  FILTER <programme>          - List students by programme (e.g FILTER Computer Science) \n");
    printf("  UNDO                        - Revert the most recent change\n");
    printf("  SAVE [filename]             - Save changes to file\n");
    printf("  HELP                        - Display this help\n");
    printf("  EXIT or QUIT                - Exit the application\n\n");

    return CMS_STATUS_OK;
}

void cms_command_loop(StudentDatabase *db)
{
    char input[CMS_MAX_COMMAND_LEN];

    while (1)
    {
        printf("CMS> ");

        if (!cms_read_line(input, CMS_MAX_COMMAND_LEN))
        {
            printf("\n");
            break;
        }

        cms_trim(input);
        if (input[0] == '\0')
        {
            continue;
        }

        if (cms_string_equals_ignore_case(input, "EXIT") ||
            cms_string_equals_ignore_case(input, "QUIT"))
        {
            printf("Exiting CMS.\n");
            break;
        }

        CMS_STATUS status = cms_parse_command(input, db);
        if (status != CMS_STATUS_OK)
        {
            cms_print_status(status);
        }
    }
}

CMS_STATUS cms_parse_command(const char *input, StudentDatabase *db)
{
    if (input == NULL || db == NULL)
    {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    cms_trim(buffer);

    if (buffer[0] == '\0')
    {
        return CMS_STATUS_OK;
    }

    char *args = NULL;
    char *command = buffer;
    char *split = buffer;
    while (*split && !isspace((unsigned char)*split))
    {
        split++;
    }
    if (*split != '\0')
    {
        *split = '\0';
        args = split + 1;
        while (*args && isspace((unsigned char)*args))
        {
            args++;
        }
        if (args[0] == '\0')
        {
            args = NULL;
        }
    }

    cms_string_to_upper(command);

    if (strcmp(command, "HELP") == 0)
    {
        if (args != NULL)
        {
            printf("HELP does not take any arguments. Ignoring extra input.\n");
        }
        return cmd_help();
    }

    if (strcmp(command, "OPEN") == 0)
    {
        const char *path = args;
        return cmd_open(db, path);
    }

    if (strcmp(command, "SHOW") == 0)
    {
        // if (args == NULL)
        // {
        //     /* Default to showing ID ascending when no arguments are given */
        //     return cmd_show(db, NULL, NULL);
        // }

        char *option = args;
        char *order = NULL;
        char *cursor = option;
        while (*cursor && !isspace((unsigned char)*cursor))
        {
            cursor++;
        }
        if (*cursor != '\0')
        {
            *cursor = '\0';
            cursor++;
            while (*cursor && isspace((unsigned char)*cursor))
            {
                cursor++;
            }
            if (*cursor != '\0')
            {
                order = cursor;
                char *order_end = order;
                while (*order_end && !isspace((unsigned char)*order_end))
                {
                    order_end++;
                }
                if (*order_end != '\0')
                {
                    *order_end = '\0';
                    order_end++;
                    while (*order_end && isspace((unsigned char)*order_end))
                    {
                        order_end++;
                    }
                    if (*order_end != '\0')
                    {
                        printf("Usage: SHOW [ALL|SUMMARY|ID|MARK|NAME|PROGRAMME] [ASC|DESC]\n");
                        return CMS_STATUS_OK;
                    }
                }
            }
        }

        return cmd_show(db, option, order);
    }

    if (strcmp(command, "INSERT") == 0)
    {
        if (args != NULL)
        {
            printf("Usage: INSERT\n");
            return CMS_STATUS_OK;
        }
        return cmd_insert(db, NULL);
    }

    if (strcmp(command, "QUERY") == 0)
    {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id))
        {
            printf("Usage: QUERY <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_query(db, student_id);
    }

    if (strcmp(command, "UPDATE") == 0)
    {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id))
        {
            printf("Usage: UPDATE <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_update(db, student_id);
    }

    if (strcmp(command, "DELETE") == 0)
    {
        int student_id = 0;
        if (!cms_parse_int_argument(args, &student_id))
        {
            printf("Usage: DELETE <student_id>\n");
            return CMS_STATUS_OK;
        }
        return cmd_delete(db, student_id);
    }

    if (strcmp(command, "FILTER") == 0)
    {
        return cmd_filter(db, args);
    }

    if (strcmp(command, "SAVE") == 0)
    {
        const char *save_path = args;
        return cmd_save(db, save_path);
    }

    if (strcmp(command, "UNDO") == 0)
    {
        if (args != NULL)
        {
            printf("Usage: UNDO\n");
            return CMS_STATUS_OK;
        }
        return cmd_undo(db);
    }

    if (strcmp(command, "EXIT") == 0 || strcmp(command, "QUIT") == 0)
    {
        printf("Type EXIT or QUIT directly at the prompt to leave the program.\n");
        return CMS_STATUS_OK;
    }

    printf("Unknown command. Type HELP to see the list of commands.\n");
    return CMS_STATUS_OK;
}
