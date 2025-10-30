#include "cms.h"

const char *cms_status_message(CmsStatus status) {
    switch (status) {
    case CMS_STATUS_OK:
        return "OK";
    case CMS_STATUS_ERROR:
        return "Error";
    case CMS_STATUS_INVALID_ARGUMENT:
        return "Invalid argument";
    case CMS_STATUS_IO:
        return "I/O error";
    case CMS_STATUS_PARSE_ERROR:
        return "Parse error";
    case CMS_STATUS_NOT_FOUND:
        return "Not found";
    case CMS_STATUS_DUPLICATE:
        return "Duplicate record";
    case CMS_STATUS_NOT_IMPLEMENTED:
        return "Not implemented";
    default:
        return "Unknown status";
    }
}
