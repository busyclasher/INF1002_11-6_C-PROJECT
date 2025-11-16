#ifndef CMS_COMMANDS_H
#define CMS_COMMANDS_H

#include "cms.h"

/* Command handler functions */
CMS_STATUS cmd_open(StudentDatabase *db, const char *filename);
CMS_STATUS cmd_show(const StudentDatabase *db, const char *option, const char *order);
CMS_STATUS cmd_insert(StudentDatabase *db, const char *params);
CMS_STATUS cmd_query(const StudentDatabase *db, int student_id);
CMS_STATUS cmd_update(StudentDatabase *db, int student_id);
CMS_STATUS cmd_delete(StudentDatabase *db, int student_id);
CMS_STATUS cmd_save(StudentDatabase *db, const char *filename);
CMS_STATUS cmd_help(void);

/* Main command loop */
void cms_command_loop(StudentDatabase *db);

/* Command parsing */
CMS_STATUS cms_parse_command(const char *input, StudentDatabase *db);

#endif /* CMS_COMMANDS_H */
