#include "commands.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "summary.h"
#include "utils.h"

typedef CmsStatus (*CommandHandler)(StudentDatabase *db, const char *args, CmsCommandResult *result);

static CmsStatus handle_open(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_show(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_insert(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_query(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_update(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_delete(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_save(StudentDatabase *db, const char *args, CmsCommandResult *result);
static CmsStatus handle_help(StudentDatabase *db, const char *args, CmsCommandResult *result);

static bool ensure_database_loaded(const StudentDatabase *db);
static CmsStatus parse_student_id_argument(const char *args, int *out_id);
static CmsStatus prompt_student_record(StudentRecord *out_record);
static CmsStatus prompt_updated_record(StudentRecord *record);
static void print_record(const StudentRecord *record);
static bool starts_with_ignore_case(const char *text, const char *prefix);

typedef struct CommandMapping {
    const char *name;
    CommandHandler handler;
} CommandMapping;

static const CommandMapping COMMANDS[] = {
    {"OPEN", handle_open},
    {"SHOW", handle_show},
    {"INSERT", handle_insert},
    {"QUERY", handle_query},
    {"UPDATE", handle_update},
    {"DELETE", handle_delete},
    {"SAVE", handle_save},
    {"HELP", handle_help},
    {"EXIT", NULL},
    {"QUIT", NULL},
};

CmsStatus cms_run_cli(StudentDatabase *db) {
    if (!db) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char line[CMS_MAX_COMMAND_LEN];
    CmsCommandResult result = CMS_COMMAND_CONTINUE;

    while (result != CMS_COMMAND_EXIT) {
        fputs("CMS> ", stdout);
        fflush(stdout);

        CmsStatus status = cms_read_line(stdin, line, sizeof(line));
        if (status == CMS_STATUS_ERROR) {
            break;
        }
        if (status != CMS_STATUS_OK) {
            fprintf(stderr, "CMS: Failed to read input (%s).\n", cms_status_message(status));
            continue;
        }

        status = cms_execute_command(db, line, &result);
        if (status != CMS_STATUS_OK && status != CMS_STATUS_NOT_IMPLEMENTED) {
            fprintf(stderr, "CMS: Command failed (%s).\n", cms_status_message(status));
        }
    }

    return CMS_STATUS_OK;
}

CmsStatus cms_execute_command(StudentDatabase *db, const char *line, CmsCommandResult *result) {
    if (!db || !line) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (result) {
        *result = CMS_COMMAND_CONTINUE;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    strncpy(buffer, line, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
    char *trimmed = cms_trim(buffer);

    if (!trimmed || trimmed[0] == '\0') {
        return CMS_STATUS_OK;
    }

    char *args = NULL;
    char *space = strchr(trimmed, ' ');
    if (space) {
        *space = '\0';
        args = cms_trim(space + 1);
    }

    cms_str_to_upper(trimmed);

    for (size_t i = 0; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); ++i) {
        if (strcmp(trimmed, COMMANDS[i].name) == 0) {
            if (!COMMANDS[i].handler) {
                if (result) {
                    *result = CMS_COMMAND_EXIT;
                }
                puts("CMS: Exiting CMS. Goodbye!");
                return CMS_STATUS_OK;
            }
            return COMMANDS[i].handler(db, args, result);
        }
    }

    printf("CMS: Unknown command \"%s\". Type HELP to list available commands.\n", trimmed);
    return CMS_STATUS_INVALID_ARGUMENT;
}

static CmsStatus handle_open(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    char path_buffer[CMS_MAX_FILE_PATH_LEN];
    const char *path = args;

    if (!path || path[0] == '\0') {
        path = CMS_DEFAULT_DATABASE_FILE;
    }

    strncpy(path_buffer, path, sizeof(path_buffer) - 1);
    path_buffer[sizeof(path_buffer) - 1] = '\0';
    cms_trim(path_buffer);

    CmsStatus status = database_open(db, path_buffer);
    if (status == CMS_STATUS_OK) {
        printf("CMS: The database file \"%s\" is successfully opened.\n", path_buffer);
        return CMS_STATUS_OK;
    }

    if (status == CMS_STATUS_IO) {
        printf("CMS: Unable to open the database file \"%s\".\n", path_buffer);
    } else if (status == CMS_STATUS_PARSE_ERROR) {
        printf("CMS: The database file \"%s\" is malformed.\n", path_buffer);
    } else {
        printf("CMS: Failed to open the database file \"%s\" (%s).\n",
               path_buffer,
               cms_status_message(status));
    }
    return status;
}

static CmsStatus handle_show(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    if (!args || args[0] == '\0') {
        puts("CMS: Missing arguments for SHOW command. Try SHOW ALL or SHOW SUMMARY.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    strncpy(buffer, args, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
    char *trimmed = cms_trim(buffer);

    if (cms_string_equals_ignore_case(trimmed, "SUMMARY")) {
        CmsStatus status = cms_show_summary(db);
        if (status == CMS_STATUS_NOT_IMPLEMENTED) {
            puts("CMS: Summary feature is not implemented yet.");
        }
        return status;
    }

    if (cms_string_equals_ignore_case(trimmed, "PROGRAMMES")) {
        CmsStatus status = cms_show_programme_stats(db);
        return status;
    }

    if (!starts_with_ignore_case(trimmed, "ALL")) {
        printf("CMS: Unknown SHOW command \"%s\".\n", trimmed);
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char *rest = trimmed + 3;
    rest = cms_trim(rest);
    if (!rest || rest[0] == '\0') {
        return cms_show_all(db);
    }

    char rest_buffer[CMS_MAX_COMMAND_LEN];
    strncpy(rest_buffer, rest, sizeof(rest_buffer));
    rest_buffer[sizeof(rest_buffer) - 1] = '\0';

    char *token = strtok(rest_buffer, " ");
    if (!token || !cms_string_equals_ignore_case(token, "SORT")) {
        puts("CMS: Expected SORT BY after SHOW ALL.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    token = strtok(NULL, " ");
    if (!token || !cms_string_equals_ignore_case(token, "BY")) {
        puts("CMS: Expected BY keyword in SHOW ALL SORT command.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    token = strtok(NULL, " ");
    if (!token) {
        puts("CMS: Missing sort key (ID or MARK).");
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CmsSortKey key;
    if (cms_string_equals_ignore_case(token, "ID")) {
        key = CMS_SORT_KEY_ID;
    } else if (cms_string_equals_ignore_case(token, "MARK")) {
        key = CMS_SORT_KEY_MARK;
    } else {
        printf("CMS: Unsupported sort key \"%s\". Use ID or MARK.\n", token);
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    CmsSortOrder order = CMS_SORT_ASC;
    token = strtok(NULL, " ");
    if (token) {
        if (cms_string_equals_ignore_case(token, "DESC")) {
            order = CMS_SORT_DESC;
            token = strtok(NULL, " ");
        } else if (cms_string_equals_ignore_case(token, "ASC")) {
            order = CMS_SORT_ASC;
            token = strtok(NULL, " ");
        } else {
            printf("CMS: Unknown sort order \"%s\". Use ASC or DESC.\n", token);
            return CMS_STATUS_INVALID_ARGUMENT;
        }

        if (token) {
            puts("CMS: Unexpected extra arguments after sort order.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
    }

    return cms_show_all_sorted(db, key, order);
}

static CmsStatus handle_insert(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)args;
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    StudentRecord record;
    CmsStatus status = prompt_student_record(&record);
    if (status != CMS_STATUS_OK) {
        return status;
    }

    status = database_insert(db, &record);
    if (status == CMS_STATUS_DUPLICATE) {
        printf("CMS: The record with ID=%d already exists.\n", record.id);
        return status;
    }
    if (status != CMS_STATUS_OK) {
        printf("CMS: Failed to insert record (%s).\n", cms_status_message(status));
        return status;
    }

    printf("CMS: New record with ID=%d is successfully inserted.\n", record.id);
    return CMS_STATUS_OK;
}

static CmsStatus handle_query(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    int student_id = 0;
    CmsStatus status = parse_student_id_argument(args, &student_id);
    if (status != CMS_STATUS_OK) {
        puts("CMS: Please provide a valid student ID (e.g., QUERY 2301234).");
        return status;
    }

    StudentRecord record;
    status = database_query(db, student_id, &record);
    if (status == CMS_STATUS_NOT_FOUND) {
        printf("CMS: The record with ID=%d does not exist.\n", student_id);
        return status;
    }
    if (status != CMS_STATUS_OK) {
        printf("CMS: Failed to query record (%s).\n", cms_status_message(status));
        return status;
    }

    print_record(&record);
    return CMS_STATUS_OK;
}

static CmsStatus handle_update(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    int student_id = 0;
    CmsStatus status = parse_student_id_argument(args, &student_id);
    if (status != CMS_STATUS_OK) {
        puts("CMS: Please provide a valid student ID (e.g., UPDATE ID=2301234).");
        return status;
    }

    StudentRecord record;
    status = database_query(db, student_id, &record);
    if (status == CMS_STATUS_NOT_FOUND) {
        printf("CMS: The record with ID=%d does not exist.\n", student_id);
        return status;
    }
    if (status != CMS_STATUS_OK) {
        printf("CMS: Unable to retrieve record for update (%s).\n", cms_status_message(status));
        return status;
    }

    printf("CMS: Updating record with ID=%d.\n", student_id);
    status = prompt_updated_record(&record);
    if (status != CMS_STATUS_OK) {
        return status;
    }

    status = database_update(db, &record);
    if (status != CMS_STATUS_OK) {
        printf("CMS: Failed to update record (%s).\n", cms_status_message(status));
        return status;
    }

    printf("CMS: The record with ID=%d is successfully updated.\n", record.id);
    return CMS_STATUS_OK;
}

static CmsStatus handle_delete(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    int student_id = 0;
    CmsStatus status = parse_student_id_argument(args, &student_id);
    if (status != CMS_STATUS_OK) {
        puts("CMS: Please provide a valid student ID (e.g., DELETE 2301234).");
        return status;
    }

    StudentRecord record;
    status = database_query(db, student_id, &record);
    if (status == CMS_STATUS_NOT_FOUND) {
        printf("CMS: The record with ID=%d does not exist.\n", student_id);
        return status;
    }
    if (status != CMS_STATUS_OK) {
        printf("CMS: Unable to load record for deletion (%s).\n", cms_status_message(status));
        return status;
    }

    print_record(&record);
    if (!cms_prompt_confirm("CMS: Are you sure you want to delete this record? Type \"Y\" to confirm: ")) {
        puts("CMS: Deletion cancelled.");
        return CMS_STATUS_OK;
    }

    bool deleted = false;
    status = database_delete(db, student_id, &deleted);
    if (status != CMS_STATUS_OK || !deleted) {
        printf("CMS: Failed to delete record (%s).\n", cms_status_message(status));
        return status;
    }

    printf("CMS: The record with ID=%d is successfully deleted.\n", student_id);
    return CMS_STATUS_OK;
}

static CmsStatus handle_save(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)result;
    if (!ensure_database_loaded(db)) {
        return CMS_STATUS_ERROR;
    }

    char path_buffer[CMS_MAX_FILE_PATH_LEN];
    const char *path = args;
    if (path && path[0] != '\0') {
        strncpy(path_buffer, path, sizeof(path_buffer) - 1);
        path_buffer[sizeof(path_buffer) - 1] = '\0';
        cms_trim(path_buffer);
        path = path_buffer;
    }

    CmsStatus status = database_save(db, path);
    if (status == CMS_STATUS_OK) {
        printf("CMS: The database file \"%s\" is successfully saved.\n", database_get_file(db));
    } else if (status == CMS_STATUS_IO) {
        printf("CMS: Failed to save database file \"%s\" due to an I/O error.\n", database_get_file(db));
    } else {
        printf("CMS: Failed to save database (%s).\n", cms_status_message(status));
    }
    return status;
}

static CmsStatus handle_help(StudentDatabase *db, const char *args, CmsCommandResult *result) {
    (void)db;
    (void)args;
    (void)result;
    puts("CMS Commands:");
    puts("  OPEN [path]           - Open the database file (default TeamName-CMS.txt).");
    puts("  SHOW ALL              - Display all student records.");
    puts("  SHOW ALL SORT BY ...  - Display records sorted by ID or MARK (ASC or DESC).");
    puts("  SHOW SUMMARY          - Display summary statistics.");
    puts("  SHOW PROGRAMMES       - Display programme counts and average marks (unique feature).");
    puts("  INSERT                - Insert a new student record.");
    puts("  QUERY <ID>            - Display a record by student ID.");
    puts("  UPDATE <ID>           - Update a record by student ID.");
    puts("  DELETE <ID>           - Delete a record by student ID.");
    puts("  SAVE [path]           - Save records to the database file.");
    puts("  EXIT                  - Exit the CMS program.");
    return CMS_STATUS_OK;
}

static bool ensure_database_loaded(const StudentDatabase *db) {
    if (!database_is_loaded(db)) {
        puts("CMS: No database file is currently open. Use OPEN to load a file.");
        return false;
    }
    return true;
}

static CmsStatus parse_student_id_argument(const char *args, int *out_id) {
    if (!out_id) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!args || args[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    strncpy(buffer, args, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
    char *trimmed = cms_trim(buffer);
    if (!trimmed || trimmed[0] == '\0') {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    if ((trimmed[0] == 'I' || trimmed[0] == 'i') &&
        (trimmed[1] == 'D' || trimmed[1] == 'd') &&
        trimmed[2] == '=') {
        trimmed += 3;
        trimmed = cms_trim(trimmed);
    }

    errno = 0;
    char *endptr = NULL;
    long value = strtol(trimmed, &endptr, 10);
    if (errno != 0 || endptr == trimmed || value <= 0) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) {
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        ++endptr;
    }

    *out_id = (int)value;
    return CMS_STATUS_OK;
}

static CmsStatus prompt_student_record(StudentRecord *out_record) {
    if (!out_record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    CmsStatus status = cms_prompt("Enter Student ID: ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    status = parse_student_id_argument(buffer, &out_record->id);
    if (status != CMS_STATUS_OK) {
        puts("CMS: Invalid student ID.");
        return status;
    }

    status = cms_prompt("Enter Student Name: ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    if (buffer[0] == '\0' || strlen(buffer) > CMS_MAX_NAME_LEN) {
        puts("CMS: Invalid student name.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    strncpy(out_record->name, buffer, sizeof(out_record->name));
    out_record->name[sizeof(out_record->name) - 1] = '\0';

    status = cms_prompt("Enter Programme: ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    if (buffer[0] == '\0' || strlen(buffer) > CMS_MAX_PROGRAMME_LEN) {
        puts("CMS: Invalid programme.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    strncpy(out_record->programme, buffer, sizeof(out_record->programme));
    out_record->programme[sizeof(out_record->programme) - 1] = '\0';

    status = cms_prompt("Enter Mark: ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    errno = 0;
    char *endptr = NULL;
    float mark = strtof(buffer, &endptr);
    if (errno != 0 || endptr == buffer) {
        puts("CMS: Invalid mark.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) {
            puts("CMS: Invalid mark.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        ++endptr;
    }
    if (mark < CMS_MIN_MARK || mark > CMS_MAX_MARK) {
        puts("CMS: Mark must be between 0 and 100.");
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    out_record->mark = mark;

    return CMS_STATUS_OK;
}

static CmsStatus prompt_updated_record(StudentRecord *record) {
    if (!record) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }

    char buffer[CMS_MAX_COMMAND_LEN];
    printf("Current Name: %s\n", record->name);
    CmsStatus status = cms_prompt("Enter new name (leave blank to keep current): ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    if (buffer[0] != '\0') {
        if (strlen(buffer) > CMS_MAX_NAME_LEN) {
            puts("CMS: Invalid name length.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        strncpy(record->name, buffer, sizeof(record->name));
        record->name[sizeof(record->name) - 1] = '\0';
    }

    printf("Current Programme: %s\n", record->programme);
    status = cms_prompt("Enter new programme (leave blank to keep current): ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    if (buffer[0] != '\0') {
        if (strlen(buffer) > CMS_MAX_PROGRAMME_LEN) {
            puts("CMS: Invalid programme length.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        strncpy(record->programme, buffer, sizeof(record->programme));
        record->programme[sizeof(record->programme) - 1] = '\0';
    }

    printf("Current Mark: %.2f\n", record->mark);
    status = cms_prompt("Enter new mark (leave blank to keep current): ", buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return status;
    }
    if (buffer[0] != '\0') {
        errno = 0;
        char *endptr = NULL;
        float mark = strtof(buffer, &endptr);
        if (errno != 0 || endptr == buffer) {
            puts("CMS: Invalid mark.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        while (*endptr != '\0') {
            if (!isspace((unsigned char)*endptr)) {
                puts("CMS: Invalid mark.");
                return CMS_STATUS_INVALID_ARGUMENT;
            }
            ++endptr;
        }
        if (mark < CMS_MIN_MARK || mark > CMS_MAX_MARK) {
            puts("CMS: Mark must be between 0 and 100.");
            return CMS_STATUS_INVALID_ARGUMENT;
        }
        record->mark = mark;
    }

    return CMS_STATUS_OK;
}

static void print_record(const StudentRecord *record) {
    if (!record) {
        return;
    }
    printf("ID: %d\n", record->id);
    printf("Name: %s\n", record->name);
    printf("Programme: %s\n", record->programme);
    printf("Mark: %.2f\n", record->mark);
}

static bool starts_with_ignore_case(const char *text, const char *prefix) {
    if (!text || !prefix) {
        return false;
    }
    while (*prefix) {
        if (*text == '\0') {
            return false;
        }
        if (toupper((unsigned char)*text) != toupper((unsigned char)*prefix)) {
            return false;
        }
        ++text;
        ++prefix;
    }
    return true;
}
