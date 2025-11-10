#include <stdio.h>
#include "cms.h"

const char* cms_status_message(CMS_STATUS status) {
    /* TODO: Implement status message mapping */
    switch (status) {
        case CMS_STATUS_OK:
            return "Operation successful";
        case CMS_STATUS_ERROR:
            return "General error occurred";
        case CMS_STATUS_INVALID_ARGUMENT:
            return "Invalid argument provided";
        case CMS_STATUS_IO:
            return "File I/O error";
        case CMS_STATUS_PARSE_ERROR:
            return "Database parsing error";
        case CMS_STATUS_NOT_FOUND:
            return "Record not found";
        case CMS_STATUS_DUPLICATE:
            return "Duplicate student ID";
        case CMS_STATUS_NOT_IMPLEMENTED:
            return "Feature not yet implemented";
        default:
            return "Unknown status";
    }
}

void cms_print_status(CMS_STATUS status) {
    /* TODO: Implement status printing */
    if (status != CMS_STATUS_OK) {
        fprintf(stderr, "Error: %s\n", cms_status_message(status));
    }
}

CMS_STATUS cms_print_declaration(void) {
    /* TODO: Implement declaration printing */
    return CMS_STATUS_NOT_IMPLEMENTED;
}

