#include <stdio.h>
#include <stdlib.h>
#include "../include/cms.h"
#include "../include/database.h"
#include "../include/commands.h"
#include "../include/utils.h"

int main(void)
{
    StudentDatabase db;
    CMS_STATUS status;

    /* TODO: Initialize database */
    status = cms_database_init(&db);
    if (status != CMS_STATUS_OK)
    {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    /* TODO: Print welcome message */
    printf("Student Records CMS\n");
    printf("Type 'HELP' for available commands\n\n");

    /* TODO: Start command loop */
    cms_command_loop(&db);

    /* TODO: Cleanup database */
    cms_database_cleanup(&db);

    return EXIT_SUCCESS;
}
