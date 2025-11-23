#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cms.h"
#include "database.h"
#include "commands.h"
#include "utils.h"

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
    printf("We hereby pledge that this project is\n");
    printf("not plagiarized and was written from our\n");
    printf("own research and compilation.\n");
    printf("-------------------------------------\n");
    printf("Key members:\n");
    printf("  Cody Tan De Xuan (2503322)\n");
    printf("  Gerome Wong (2503678)\n");
    printf("  Gabriel Lee Hon Fei (2501901)\n");
    printf("  Natalie Peh (2501526)\n");
    printf("  Randy See Meng Teck (2500605)\n");
    printf("-------------------------------------\n");
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

    cms_command_loop(&db);

    if (db.is_dirty) {
        char response[8];
        while (1) {
            printf("Unsaved changes detected. Save before exiting? (Y/N): ");
            if (!cms_read_line(response, sizeof(response))) {
                printf("\nInput error detected. Exiting without saving.\n");
                break;
            }

            if (response[0] == '\0') {
                continue;
            }

            char choice = (char)toupper((unsigned char)response[0]);
            if (choice == 'Y') {
                CMS_STATUS save_status = cmd_save(&db, NULL);
                if (save_status == CMS_STATUS_OK) {
                    printf("Changes saved successfully.\n");
                } else {
                    printf("Failed to save changes.\n");
                    cms_print_status(save_status);
                }
                break;
            } else if (choice == 'N') {
                printf("Changes not saved.\n");
                break;
            } else {
                printf("Please enter 'Y' or 'N'.\n");
            }
        }
    }

    cms_database_cleanup(&db);

    return EXIT_SUCCESS;
}
