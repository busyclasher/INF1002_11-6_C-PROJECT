#ifndef CMS_CONFIG_H
#define CMS_CONFIG_H

/* Maximum field lengths */
#define CMS_MAX_NAME_LEN 64
#define CMS_MAX_PROGRAMME_LEN 64
#define CMS_MAX_COMMAND_LEN 256
#define CMS_MAX_FILE_PATH_LEN 260

/* Mark constraints */
#define CMS_MIN_MARK 0.0
#define CMS_MAX_MARK 100.0

/* Default database file */
#define CMS_DEFAULT_DATABASE_FILE "TeamName-CMS.txt"

/* Database capacity settings */
#define CMS_INITIAL_CAPACITY 16
#define CMS_GROWTH_FACTOR 2

#endif /* CMS_CONFIG_H */

