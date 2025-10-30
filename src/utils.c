#include "utils.h"

#include <ctype.h>
#include <string.h>

char *cms_trim(char *str) {
    if (!str) {
        return NULL;
    }
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        ++start;
    }

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        --end;
    }
    *end = '\0';

    if (start != str) {
        memmove(str, start, (size_t)(end - start) + 1);
    }
    return str;
}

void cms_str_to_upper(char *str) {
    if (!str) {
        return;
    }
    for (; *str; ++str) {
        *str = (char)toupper((unsigned char)*str);
    }
}

bool cms_string_equals_ignore_case(const char *a, const char *b) {
    if (!a || !b) {
        return false;
    }
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) {
            return false;
        }
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

CmsStatus cms_read_line(FILE *stream, char *buffer, size_t capacity) {
    if (!stream || !buffer || capacity == 0) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (!fgets(buffer, (int)capacity, stream)) {
        return feof(stream) ? CMS_STATUS_ERROR : CMS_STATUS_IO;
    }
    cms_trim(buffer);
    return CMS_STATUS_OK;
}

CmsStatus cms_prompt(const char *prompt, char *buffer, size_t capacity) {
    if (!buffer || capacity == 0) {
        return CMS_STATUS_INVALID_ARGUMENT;
    }
    if (prompt) {
        fputs(prompt, stdout);
    }
    return cms_read_line(stdin, buffer, capacity);
}

bool cms_prompt_confirm(const char *prompt) {
    char buffer[8];
    CmsStatus status = cms_prompt(prompt, buffer, sizeof(buffer));
    if (status != CMS_STATUS_OK) {
        return false;
    }
    return buffer[0] == 'Y' || buffer[0] == 'y';
}

CmsStatus cms_print_declaration(void) {
    return CMS_STATUS_NOT_IMPLEMENTED;
}
