#ifndef CMS_H
#define CMS_H

#include <stddef.h>

typedef enum CmsStatus {
    CMS_STATUS_OK = 0,
    CMS_STATUS_ERROR,
    CMS_STATUS_INVALID_ARGUMENT,
    CMS_STATUS_IO,
    CMS_STATUS_PARSE_ERROR,
    CMS_STATUS_NOT_FOUND,
    CMS_STATUS_DUPLICATE,
    CMS_STATUS_NOT_IMPLEMENTED
} CmsStatus;

const char *cms_status_message(CmsStatus status);

#endif /* CMS_H */
