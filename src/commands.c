#include "commands.h"

#include <stdio.h>

#include "summary.h"
#include "utils.h"

CmsStatus cms_run_cli(StudentDatabase *db) {
    (void)db;
    return CMS_STATUS_NOT_IMPLEMENTED;
}

CmsStatus cms_execute_command(StudentDatabase *db, const char *line, CmsCommandResult *result) {
    (void)db;
    (void)line;
    if (result) {
        *result = CMS_COMMAND_CONTINUE;
    }
    return CMS_STATUS_NOT_IMPLEMENTED;
}
