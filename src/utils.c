#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "config.h"

bool cms_validate_student_id(int id) {
    /* TODO: Implement student ID validation */
    /* Should be a positive integer, typically 7 digits */
    return id > 0;
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
    if (str == NULL) {
        return;
    }

    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0';

    if (start != str) {
        memmove(str, start, (size_t)(end - start + 1));
    }
}

void cms_trim(char *str) {
    cms_trim_string(str);
}

bool cms_string_equals_ignore_case(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }

    return *a == *b;
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

