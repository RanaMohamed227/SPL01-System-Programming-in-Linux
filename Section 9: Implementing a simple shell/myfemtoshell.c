#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int femtoshell_main(int argc, char *argv[]) {
    char input[BUFFER_SIZE];

    while (1) {
        printf("MiniShell >> ");
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            printf("Error reading input\n");
            continue;
        }
        input[strcspn(input, "\n")] = '\0';


        if (strcmp(input, "exit") == 0) {
            printf("Good Bye :)\n");
            break;
        }

     
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
