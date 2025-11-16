#ifndef CMS_H
#define CMS_H

#include <stdbool.h>
#include <stddef.h>
#include "config.h"

/* Status codes for CMS operations */
typedef enum
{
    CMS_STATUS_OK = 0,
    CMS_STATUS_ERROR,
    CMS_STATUS_INVALID_ARGUMENT,
    CMS_STATUS_IO,
    CMS_STATUS_PARSE_ERROR,
    CMS_STATUS_NOT_FOUND,
    CMS_STATUS_DUPLICATE,
    CMS_STATUS_NOT_IMPLEMENTED
} CMS_STATUS;

/* Student record structure */
typedef struct StudentRecord
{
    int id;
    char name[CMS_MAX_NAME_LEN + 1];
    char programme[CMS_MAX_PROGRAMME_LEN + 1];
    float mark;
} StudentRecord;

/* Database structure */
typedef struct StudentDatabase
{
    StudentRecord *records;
    size_t count;
    size_t capacity;
    char file_path[CMS_MAX_FILE_PATH_LEN];
    bool is_loaded;
    bool is_dirty;
} StudentDatabase;

/* Status message handling */
const char *cms_status_message(CMS_STATUS status);
void cms_print_status(CMS_STATUS status);

/* Declaration function (not yet implemented) */
CMS_STATUS cms_print_declaration(void);

#endif /* CMS_H */
