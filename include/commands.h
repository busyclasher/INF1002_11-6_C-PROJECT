#ifndef COMMANDS_H
#define COMMANDS_H

#include "cms.h"
#include "database.h"

typedef enum CmsCommandResult {
    CMS_COMMAND_CONTINUE = 0,
    CMS_COMMAND_EXIT
} CmsCommandResult;

CmsStatus cms_run_cli(StudentDatabase *db);
CmsStatus cms_execute_command(StudentDatabase *db, const char *line, CmsCommandResult *result);

#endif /* COMMANDS_H */
