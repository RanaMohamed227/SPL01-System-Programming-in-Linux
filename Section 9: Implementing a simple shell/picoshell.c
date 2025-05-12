#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int picoshell_main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int last_status = 0;

    for (;;) {
        /* 1) Prompt */
        if (printf("Pico shell prompt > ") < 0) {
            perror("printf");
            return last_status;
        }
        if (fflush(stdout) == EOF) {
            perror("fflush");
            return last_status;
        }

        /* 2) Read a line */
        len = getline(&line, &cap, stdin);
        if (len < 0) {
            if (feof(stdin)) {
                /* at EOF: just print a newline and break */
                putchar('\n');
                fflush(stdout);
                break;
            } else {
                perror("getline");
                last_status = 1;
                continue;
            }
        }

        /* 3) Strip trailing newline */
        if (len > 0 && line[len-1] == '\n') {
            line[--len] = '\0';
        }

        /* 4) Blank‐line? echo a newline and re‐prompt */
        size_t i = 0;
        while (i < (size_t)len && line[i] == ' ') i++;
        if (i == (size_t)len) {
            putchar('\n');
            fflush(stdout);
            last_status = 0;
            continue;
        }

        /* 5) Non‐blank → simulate ENTER (newline + flush) */
        putchar('\n');
        fflush(stdout);

        /* 6) Tokenize into args[] (space‐separated) */
        size_t argv_cap = 8, argcnt = 0;
        char **args = (char**)malloc(argv_cap * sizeof(char*));
        if (!args) {
            perror("malloc");
            last_status = 1;
            continue;
        }
        char *saveptr = NULL;
        char *tok = strtok_r(line, " ", &saveptr);
        while (tok) {
            if (argcnt + 1 >= argv_cap) {
                argv_cap *= 2;
                char **tmp = (char**)realloc(args, argv_cap * sizeof(char*));
                if (!tmp) {
                    perror("realloc");
                    free(args);
                    last_status = 1;
                    args = NULL;
                    break;
                }
                args = tmp;
            }
            args[argcnt++] = tok;
            tok = strtok_r(NULL, " ", &saveptr);
        }
        if (!args) continue;
        args[argcnt] = NULL;

        /* 7) Built‐ins */
        if (strcmp(args[0], "exit") == 0) {
            free(args);
            printf("Good Bye\n");
            fflush(stdout);
            free(line);
            return last_status;
        }
        else if (strcmp(args[0], "cd") == 0) {
            const char *dir = (argcnt > 1) ? args[1] : getenv("HOME");
            if (!dir) dir = "/";
            if (chdir(dir) != 0) {
                fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
                last_status = 1;
            } else {
                last_status = 0;
            }
        }
        else if (strcmp(args[0], "pwd") == 0) {
            char cwd[4096];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                perror("getcwd");
                last_status = 1;
            } else {
                printf("%s\n", cwd);
                last_status = 0;
            }
        }
        else if (strcmp(args[0], "echo") == 0) {
            for (size_t j = 1; j < argcnt; j++) {
                if (j > 1) putchar(' ');
                fputs(args[j], stdout);
            }
            putchar('\n');
            last_status = 0;
        }
        /* 8) External commands */
        else {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                last_status = 1;
            }
            else if (pid == 0) {
                execvp(args[0], args);
                /* on failure: print to stdout, then _exit(127) */
                char errbuf[512];
                int n = snprintf(errbuf, sizeof(errbuf),
                                 "%s: command not found\n", args[0]);
                if (n > 0) {
                    write(STDOUT_FILENO, errbuf, (size_t)n);
                }
                _exit(127);
            }
            else {
                int status;
                if (waitpid(pid, &status, 0) < 0) {
                    perror("waitpid");
                    last_status = 1;
                } else if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                } else {
                    last_status = 1;
                }
            }
        }

        /* 9) Clean up */
        free(args);
    }

    free(line);
    return last_status;
}
