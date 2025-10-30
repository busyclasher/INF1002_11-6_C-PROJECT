#include "utils.h"

#include <ctype.h>
#include <stdbool.h>
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

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] != '\n') {
        int ch = 0;
        while ((ch = fgetc(stream)) != '\n' && ch != EOF) {
            /* discard remainder */
        }
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
        fflush(stdout);
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
    static const char *lines[] = {
        "",
        "Declaration",
        "SIT's policy on copying does not allow the students to copy source code as well as assessment",
        "solutions from another person, AI, or other places. It is the students' responsibility to guarantee",
        "that their assessment solutions are their own work. Meanwhile, the students must also ensure that",
        "their work is not accessible by others. Where such plagiarism is detected, both of the assessments",
        "involved will receive ZERO mark.",
        "",
        "We hereby declare that:",
        "1. We fully understand and agree to the abovementioned plagiarism policy.",
        "2. We did not copy any code from others or from other places.",
        "3. We did not share our codes with others or upload to any other places for public access and will",
        "   not do that in the future.",
        "4. We agree that our project will receive zero mark if there is any plagiarism detected.",
        "5. We agree that we will not disclose any information or material of the group project to others or",
        "   upload to any other places for public access.",
        "6. We agree that we did not copy any code directly from AI generated sources.",
        "",
        "Declared by: Group Name (please insert your group name)",
        "Team members:",
        "1. ________________________________",
        "2. ________________________________",
        "3. ________________________________",
        "4. ________________________________",
        "5. ________________________________",
        "Date: (please insert the submission date).",
        ""
    };

    for (size_t i = 0; i < sizeof(lines) / sizeof(lines[0]); ++i) {
        puts(lines[i]);
    }

    return CMS_STATUS_OK;
}
