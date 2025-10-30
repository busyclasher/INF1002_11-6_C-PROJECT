#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "cms.h"

char *cms_trim(char *str);
void cms_str_to_upper(char *str);
bool cms_string_equals_ignore_case(const char *a, const char *b);

CmsStatus cms_read_line(FILE *stream, char *buffer, size_t capacity);
CmsStatus cms_prompt(const char *prompt, char *buffer, size_t capacity);
bool cms_prompt_confirm(const char *prompt);

CmsStatus cms_print_declaration(void);

#endif /* UTILS_H */
