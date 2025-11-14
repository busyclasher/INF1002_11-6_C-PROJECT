#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/utils.h"
#include "../include/config.h"

bool cms_validate_student_id(int id) {
    // Basic validation: checks if the id is 7 digit.
    int dividor = 1000000; // 1 and 6 zeroes
    int result = id / dividor;
    if (result < 1 || result > 9) {
        return false;
    } else {
        return true;
    }
}



bool cms_validate_name(const char *name) {
    /* TODO: Implement name validation */
    if (name == NULL || strlen(name) == 0) {
        return false;
    }
    
    if (strlen(name) > CMS_MAX_NAME_LEN) {
        return false;
    }

    return true;
}

bool cms_validate_programme(const char *programme) {
    /* TODO: Implement programme validation */
    if (programme == NULL || strlen(programme) == 0) {
        return false;
    }
    
    if (strlen(programme) > CMS_MAX_PROGRAMME_LEN) {
        return false;
    }

    return true;
}

bool cms_validate_mark(float mark) {
    /* TODO: Implement mark validation */
    return mark >= CMS_MIN_MARK && mark <= CMS_MAX_MARK;
}

void cms_trim_string(char *str) {
    /* Remove leading and trailing whitespace */
    if (str == NULL) {
        return;
    }

    int len = strlen(str);
    
    /* Remove leading whitespace */
    int start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }

    /* Remove trailing whitespace */
    int end = len - 1;
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;
    }

    /* Shift trimmed string and null-terminate */
    if (start > 0) {
        memmove(str, str + start, end - start + 2);
    }
    str[end - start + 1] = '\0';
}

void cms_string_to_upper(char *str) {
    /* TODO: Implement string to uppercase conversion */
    if (str == NULL) {
        return;
    }

    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

bool cms_read_line(char *buffer, size_t size) {
    /* TODO: Implement line reading with proper handling */
    if (buffer == NULL || size == 0) {
        return false;
    }

    if (fgets(buffer, size, stdin) == NULL) {
        return false;
    }

    /* Remove newline if present */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    return true;
}

bool cms_read_int(const char *prompt, int *value) {
    /* TODO: Implement integer reading with validation */
    if (prompt == NULL || value == NULL) {
        return false;
    }

    printf("%s", prompt);
    
    if (scanf("%d", value) != 1) {
        /* Clear input buffer */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }

    /* Clear remaining input */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return true;
}

bool cms_read_float(const char *prompt, float *value) {
    /* TODO: Implement float reading with validation */
    if (prompt == NULL || value == NULL) {
        return false;
    }

    printf("%s", prompt);
    
    if (scanf("%f", value) != 1) {
        /* Clear input buffer */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }

    /* Clear remaining input */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return true;
}

bool cms_read_string(const char *prompt, char *buffer, size_t size) {
    /* TODO: Implement string reading with validation */
    if (prompt == NULL || buffer == NULL || size == 0) {
        return false;
    }

    printf("%s", prompt);
    return cms_read_line(buffer, size);
}

/**
 * Displays a formatted table of student records with ASCII borders.
 * @param db Pointer to StudentDatabase structure containing records to display.
 */
void cms_display_table(const StudentDatabase *db) {
    if (db == NULL) {
        return;
    }

    int id_width = 12;
    int name_width = 20;
    int prog_width = 30;
    int mark_width = 10;
    
    /* Print table header border */
    printf("\n+%.*s+%.*s+%.*s+%.*s+\n", 
           id_width, "------------", 
           name_width, "--------------------", 
           prog_width, "------------------------------", 
           mark_width, "----------");
    
    /* Print column headers */
    printf("| %-*s| %-*s| %-*s| %-*s|\n", 
           id_width - 1, "ID", 
           name_width - 1, "Name", 
           prog_width - 1, "Programme", 
           mark_width - 1, "Mark");
    printf("+%.*s+%.*s+%.*s+%.*s+\n", 
           id_width, "------------", 
           name_width, "--------------------", 
           prog_width, "------------------------------", 
           mark_width, "----------");
    
    /* Print each student record */
    for (size_t i = 0; i < db->count; i++) {
        const StudentRecord *rec = &db->records[i];
        printf("| %-*d| %-*s| %-*s| %-*.1f|\n",
               id_width - 1, rec->id,
               name_width - 1, rec->name,
               prog_width - 1, rec->programme,
               mark_width - 1, rec->mark);
    }
    
    /* Print table footer border */
    printf("+%.*s+%.*s+%.*s+%.*s+\n\n", 
           id_width, "------------", 
           name_width, "--------------------", 
           prog_width, "------------------------------", 
           mark_width, "----------");
}

