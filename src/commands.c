#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "commands.h"
#include "database.h"
#include "summary.h"
#include "utils.h"
#include "config.h"

static bool cms_parse_int_argument(const char *text, int *out_value) {
    if (text == NULL || out_value == NULL) {
        return false;
    }

    while (*text && isspace((unsigned char)*text)) {
        text++;
    }

    if (*text == '\0') {
        return false;
    }

    errno = 0;
    char *end_ptr = NULL;
    long value = strtol(text, &end_ptr, 10);
    if (errno != 0) {
        return false;
    }

    while (end_ptr && *end_ptr && isspace((unsigned char)*end_ptr)) {
        end_ptr++;
    }

    if (end_ptr != NULL && *end_ptr != '\0') {
        return false;
    }

    if (value < INT_MIN || value > INT_MAX) {
        return false;
    }

    *out_value = (int)value;
    return true;
}

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
    /* Default to sorting by ID ascending if no option is provided */
    if (option == NULL) {
        option = "ID";
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

        return cms_show_all_sorted(db, CMS_SORT_KEY_ID, sort_order);
    }

    CmsSortKey sort_key;
    if (cms_string_equals_ignore_case(opt_buf, "ID")) {
        sort_key = CMS_SORT_KEY_ID;
    } else if (cms_string_equals_ignore_case(opt_buf, "MARK")) {
        sort_key = CMS_SORT_KEY_MARK;
    } else if (cms_string_equals_ignore_case(opt_buf, "NAME")) {
        sort_key = CMS_SORT_KEY_NAME;
    } else if (cms_string_equals_ignore_case(opt_buf, "PROGRAMME") ||
               cms_string_equals_ignore_case(opt_buf, "PROGRAM")) {
        sort_key = CMS_SORT_KEY_PROGRAMME;
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
    if (db == NULL) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    StudentRecord new_record;

    /* Read student ID */
    if (!cms_read_int("Enter student ID: ", &new_record.id)) {
        printf("Failed to read student ID.\n");
        return CMS_STATUS_ERROR;
    }

    if (!cms_validate_student_id(new_record.id)) {
        printf("Invalid student ID.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Read name */
    if (!cms_read_string("Enter name: ", new_record.name, sizeof(new_record.name))) {
        printf("Failed to read name.\n");
        return CMS_STATUS_ERROR;
    }

    if (!cms_validate_name(new_record.name)) {
        printf("Invalid name.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Read programme */
    if (!cms_read_string("Enter programme: ", new_record.programme, sizeof(new_record.programme))) {
        printf("Failed to read programme.\n");
        return CMS_STATUS_ERROR;
    }

    if (!cms_validate_programme(new_record.programme)) {
        printf("Invalid programme.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    /* Read mark */
    if (!cms_read_float("Enter mark (0-100): ", &new_record.mark)) {
        printf("Failed to read mark.\n");
        return CMS_STATUS_ERROR;
    }

    if (!cms_validate_mark(new_record.mark)) {
        printf("Invalid mark.\n");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CMS_STATUS status = cms_database_insert(db, &new_record);
    if (status == CMS_STATUS_DUPLICATE) {
        printf("A record with ID %d already exists.\n", new_record.id);
    } else if (status == CMS_STATUS_OK) {
        printf("Record inserted successfully.\n");
    } else {
        cms_print_status(status);
    }

    return status;
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

