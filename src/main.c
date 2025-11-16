#include <stdio.h>
#include <stdlib.h>
#include "cms.h"
#include "database.h"
#include "commands.h"

int main(void) {
    StudentDatabase db;
    CMS_STATUS status;

    /* TODO: Initialize database */
    status = cms_database_init(&db);
    if (status != CMS_STATUS_OK) {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    /* Print welcome message */
    printf("=====================================\n");
    printf("       Student Records CMS\n");
    printf("=====================================\n");
    printf("Type 'HELP' for the list of commands.\n");
    printf("Default database: %s\n\n", CMS_DEFAULT_DATABASE_FILE);

    /* Attempt to open default database file */
    printf("Loading default database...\n");
    status = cmd_open(&db, CMS_DEFAULT_DATABASE_FILE);
    if (status == CMS_STATUS_OK) {
        printf("Successfully loaded '%s'.\n\n", CMS_DEFAULT_DATABASE_FILE);
    } else {
        printf("Warning: Failed to load '%s'.\n", CMS_DEFAULT_DATABASE_FILE);
        cms_print_status(status);
        printf("You may use the OPEN command to load another file.\n\n");
    }

    /* TODO: Start command loop */
    cms_command_loop(&db);

    /* TODO: Cleanup database */
    cms_database_cleanup(&db);

    return EXIT_SUCCESS;
}

