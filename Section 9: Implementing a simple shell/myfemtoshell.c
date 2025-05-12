#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int femtoshell_main(int argc, char *argv[])
{
    char input[90000];
    int last_status = 0;
    (void)argc;
    (void)argv;

    for (;;) {
        /* 1) Prompt */
        if (printf("miniShell> ") < 0) {
            perror("printf");
            return EXIT_FAILURE;
        }
        if (fflush(stdout) == EOF) {
            perror("fflush");
            return EXIT_FAILURE;
        }

        /* 2) Read a line */
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                /* EOF: print one final newline and exit loop */
                putchar('\n');
                break;
            } else {
                perror("fgets");
                last_status = 0;
                continue;
            }
        }

        /* 3) Strip trailing newline */
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[--len] = '\0';
        }

        /* 4) Blank line → just echo back a newline */
        if (len == 0) {
            putchar('\n');
            last_status = 0;
            continue;
        }

        /* 5) exit */
        if (strcmp(input, "exit") == 0) {
            putchar('\n');
            if (printf("Good Bye\n") < 0)
                perror("printf");
            return EXIT_SUCCESS;   /* always return 0 */
        }

        /* 6) echo */
        if (strncmp(input, "echo", 4) == 0) {
            if (len == 4) {
                /* “echo” alone → blank line */
                putchar('\n');
                last_status = 0;
            }
            else if (input[4] == ' ') {
                putchar('\n');
                if (printf("%s\n", input + 5) < 0) {
                    perror("printf");
                    last_status = 0;
                } else {
                    last_status = 0;
                }
            }
            else {
                /* malformed “echoX” */
                putchar('\n');
                if (printf("Invalid command\n") < 0)
                    perror("printf");
                last_status = 0;
            }
            continue;
        }

        /* 7) anything else → invalid */
        putchar('\n');
        if (printf("Invalid command\n") < 0)
            perror("printf");
        last_status = 1;
    }

    /* EOF reached without “exit” */
    return last_status;   /* always return 0 */
}
