#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cms.h"
#include "database.h"
#include "commands.h"
#include "utils.h"

int main(void)
{
    StudentDatabase db;
    CMS_STATUS status;

    status = cms_database_init(&db);
    if (status != CMS_STATUS_OK)
    {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    printf("Declaration\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assessment solutions\n");
    printf("from another person AI or other places. It is the students' responsibility to guarantee that their\n");
    printf("assessment solutions are their own work. Meanwhile, the students must also ensure that their work is\n");
    printf("not accessible by others. Where such plagiarism is detected, both of the assessments involved will\n");
    printf("receive ZERO mark.\n\n");
    printf("We hereby declare that:\n");
    printf("- We fully understand and agree to the abovementioned plagiarism policy.\n");
    printf("- We did not copy any code from others or from other places.\n");
    printf("- We did not share our codes with others or upload to any other places for public access and will\n");
    printf("not do that in the future.\n");
    printf("- We agree that our project will receive Zero mark if there is any plagiarism detected.\n");
    printf("- We agree that we will not disclose any information or material of the group project to others or\n");
    printf("upload to any other places for public access.\n");
    printf("- We agree that we did not copy any code directly from AI generated sources\n\n");
    printf("Declared by: Phisherman\n");
    printf("Team members:\n");
    printf("1. Cody Tan De Xuan (2503322)\n");
    printf("2. Gerome Wong (2503678)\n");
    printf("3. Gabriel Lee Hon Fei (2501901)\n");
    printf("4. Natalie Peh (2501526)\n");
    printf("5. Randy See Meng Teck (2500605)\n");
    printf("Date: 23/11/2025\n\n");
    printf("Type 'HELP' for the list of commands.\n");
    printf("Default database: %s\n\n", CMS_DEFAULT_DATABASE_FILE);

    /* Attempt to open default database file */
    printf("Loading default database...\n");
    status = cmd_open(&db, CMS_DEFAULT_DATABASE_FILE);
    if (status == CMS_STATUS_OK)
    {
        printf("Successfully loaded '%s'.\n\n", CMS_DEFAULT_DATABASE_FILE);
    }
    else
    {
        printf("Warning: Failed to load '%s'.\n", CMS_DEFAULT_DATABASE_FILE);
        cms_print_status(status);
        printf("You may use the OPEN command to load another file.\n\n");
    }

    cms_command_loop(&db);

    if (db.is_dirty)
    {
        char response[8];
        while (1)
        {
            printf("Unsaved changes detected. Save before exiting? (Y/N): ");
            if (!cms_read_line(response, sizeof(response)))
            {
                printf("\nInput error detected. Exiting without saving.\n");
                break;
            }

            if (response[0] == '\0')
            {
                continue;
            }

            char choice = (char)toupper((unsigned char)response[0]);
            if (choice == 'Y')
            {
                CMS_STATUS save_status = cmd_save(&db, NULL);
                if (save_status == CMS_STATUS_OK)
                {
                    printf("Changes saved successfully.\n");
                }
                else
                {
                    printf("Failed to save changes.\n");
                    cms_print_status(save_status);
                }
                break;
            }
            else if (choice == 'N')
            {
                printf("Changes not saved.\n");
                break;
            }
            else
            {
                printf("Please enter 'Y' or 'N'.\n");
            }
        }
    }

    cms_database_cleanup(&db);

    return EXIT_SUCCESS;
}
