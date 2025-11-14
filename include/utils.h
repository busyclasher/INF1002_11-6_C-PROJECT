#ifndef CMS_UTILS_H
#define CMS_UTILS_H

#include "cms.h"
#include <stdbool.h>

/* Input validation */
bool cms_validate_student_id(int id);
bool cms_validate_name(const char *name);
bool cms_validate_programme(const char *programme);
bool cms_validate_mark(float mark);

/* String utilities */
void cms_trim_string(char *str);
void cms_string_to_upper(char *str);

/* Input reading */
bool cms_read_line(char *buffer, size_t size);
bool cms_read_int(const char *prompt, int *value);
bool cms_read_float(const char *prompt, float *value);
bool cms_read_string(const char *prompt, char *buffer, size_t size);

/* Table display */
void cms_display_table(const StudentDatabase *db);

#endif /* CMS_UTILS_H */

