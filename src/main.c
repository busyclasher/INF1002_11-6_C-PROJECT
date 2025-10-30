#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "database.h"
#include "utils.h"

int main(void) {
    StudentDatabase db;
    CmsStatus status = database_init(&db);
    if (status != CMS_STATUS_OK) {
        fprintf(stderr, "CMS: Failed to initialize database (%s).\n", cms_status_message(status));
        return EXIT_FAILURE;
    }

    status = cms_print_declaration();
    if (status != CMS_STATUS_OK) {
        fprintf(stderr, "CMS: Failed to print declaration (%s).\n", cms_status_message(status));
        database_free(&db);
        return EXIT_FAILURE;
    }

    puts("CMS: Type HELP to list available commands.");

    status = cms_run_cli(&db);
    if (status != CMS_STATUS_OK) {
        fprintf(stderr, "CMS: Command loop terminated with error (%s).\n", cms_status_message(status));
        database_free(&db);
        return EXIT_FAILURE;
    }

    database_free(&db);
    return EXIT_SUCCESS;
}
