#include <stdio.h>
#include "../include/cms.h"

const char *cms_status_message(CMS_STATUS status)
{
    switch (status)
    {
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

void cms_print_status(CMS_STATUS status)
{
    if (status != CMS_STATUS_OK)
    {
        fprintf(stderr, "Error: %s\n", cms_status_message(status));
    }
}

CMS_STATUS cms_print_declaration(void)
{
    printf("Declaration\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assessment solutions\n");
    printf("from another person AI or other places. Meanwhile, the students must also ensure that their work is\n");
    printf("not accessible by others.\n\n");
    printf("We hereby declare that:\n");
    printf("- We fully understand and agree to the abovementioned plagiarism policy.\n");
    printf("- We did not copy any code from others or from other places.\n");
    printf("- We did not share our codes with others or upload to any other places for public access and will\n");
    printf("not do that in the future.\n");
    printf("- We agree that our project will receive Zero mark if there is any plagiarism detected.\n");
    printf("- We agree that we will not disclose any information or material of the group project to others or\n");
    printf("upload to any other places for public access.\n");
    printf("- We agree that we did not copy any code directly from AI generated sources\n\n");
    printf("Declared by: Team P2-6\n");
    printf("Team members:\n");
    printf("1. Cody Tan De Xuan (2503322)\n");
    printf("2. Gerome Wong (2503678)\n");
    printf("3. Gabriel Lee Hon Fei (2501901)\n");
    printf("4. Natalie Peh (2501526)\n");
    printf("5. Randy See Meng Teck (2500605)\n");
    printf("Date: 23/11/2025\n\n");
    return 1;
}
