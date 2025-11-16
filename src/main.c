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

    /* TODO: Start command loop */
    cms_command_loop(&db);

    /* TODO: Cleanup database */
    cms_database_cleanup(&db);

    return EXIT_SUCCESS;
}

