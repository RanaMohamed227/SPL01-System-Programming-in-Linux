#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>    /* for isalpha, isalnum */

/* ── 1) Data structures for shell variables ───────────────────────────────── */

typedef struct {
    char *name;
    char *value;
    int   exported;   /* 0 = local-only; 1 = marked for export */
} shell_var_t;

static shell_var_t *vars     = NULL;
static size_t       n_vars   = 0,
                    cap_vars = 0;

/* ── 2) Helper functions ────────────────────────────────────────────────────── */

/* Find index of a variable by name, or –1 if not found */
static int find_var(const char *name) {
    for (size_t i = 0; i < n_vars; i++) {
        if (strcmp(vars[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

/* Set or update a variable; returns 0 on success, -1 on malloc error */
static int set_var(const char *name, const char *value) {
    int idx = find_var(name);
    if (idx >= 0) {
        free(vars[idx].value);
        vars[idx].value = strdup(value);
        return vars[idx].value ? 0 : -1;
    }
    if (n_vars == cap_vars) {
        size_t new_cap = cap_vars ? cap_vars * 2 : 8;
        shell_var_t *tmp = (shell_var_t *)realloc(vars, new_cap * sizeof(*vars));
        if (!tmp) return -1;
        vars = tmp;
        cap_vars = new_cap;
    }
    vars[n_vars].name     = strdup(name);
    vars[n_vars].value    = strdup(value);
    vars[n_vars].exported = 0;
    if (!vars[n_vars].name || !vars[n_vars].value)
        return -1;
    n_vars++;
    return 0;
}

/* Lookup a variable’s value, or NULL if unset */
static const char *get_var(const char *name) {
    int idx = find_var(name);
    return (idx >= 0) ? vars[idx].value : NULL;
}

/* Free all variables (call at shell exit) */
static void free_vars(void) {
    for (size_t i = 0; i < n_vars; i++) {
        free(vars[i].name);
        free(vars[i].value);
    }
    free(vars);
    vars = NULL;
    n_vars = cap_vars = 0;
}

/* ── 2a) New helper: expand all $VAR occurrences inside a string ──────────── */
static char *expand_vars_in_str(const char *str) {
    size_t bufcap = strlen(str) + 1;
    char *out = (char *)malloc(bufcap);
    if (!out) return NULL;

    size_t ri = 0;  /* index in out */
    for (size_t i = 0; str[i]; ) {
        if (str[i] == '$' &&
            (isalpha((unsigned char)str[i+1]) || str[i+1] == '_'))
        {
            /* parse variable name */
            size_t j = i + 1;
            while (isalnum((unsigned char)str[j]) || str[j] == '_') j++;
            size_t namelen = j - (i + 1);

            char name[namelen + 1];
            memcpy(name, &str[i + 1], namelen);
            name[namelen] = '\0';

            /* lookup value (or "") */
            const char *val = get_var(name);
            if (!val) val = "";

            size_t vall = strlen(val);
            /* ensure capacity for val + remainder */
            size_t need = ri + vall + strlen(str + j) + 1;
            if (need > bufcap) {
                bufcap = need * 2;
                char *tmp = (char *)realloc(out, bufcap);
                if (!tmp) { free(out); return NULL; }
                out = tmp;
            }
            memcpy(out + ri, val, vall);
            ri += vall;
            i = j;
        } else {
            /* copy literal character */
            if (ri + 2 > bufcap) {
                bufcap = bufcap * 2 + 2;
                char *tmp = (char *)realloc(out, bufcap);
                if (!tmp) { free(out); return NULL; }
                out = tmp;
            }
            out[ri++] = str[i++];
        }
    }
    out[ri] = '\0';
    return out;
}

/* ── 3) The shell “main” ───────────────────────────────────────────────────── */
int nanoshell_main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int last_status = 0;

    for (;;) {
        /* 1) Prompt */
        if (printf("Pico shell prompt > ") < 0) {
            perror("printf");
            free_vars();
            return last_status;
        }
        if (fflush(stdout) == EOF) {
            perror("fflush");
            free_vars();
            return last_status;
        }

        /* 2) Read a line */
        len = getline(&line, &cap, stdin);
        if (len < 0) {
            if (feof(stdin)) {
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
        if (len > 0 && line[len-1] == '\n')
            line[--len] = '\0';

        /* 4) Blank‐line? echo newline & re‐prompt */
        size_t i = 0;
        while (i < (size_t)len && line[i] == ' ') i++;
        if (i == (size_t)len) {
            putchar('\n');
            fflush(stdout);
            last_status = 0;
            continue;
        }

        /* 5) Echo ENTER (newline + flush) */
        putchar('\n');
        fflush(stdout);

        /* 6) Tokenize into args[] */
        size_t argv_cap = 8, argcnt = 0;
        char **args = (char **)malloc(argv_cap * sizeof(char*));
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
                char **tmp = (char **)realloc(args, argv_cap * sizeof(char*));
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

        /* ── 3a) Simple local‐assignment? ───────────────────────────── */
        if (argcnt == 1) {
            char *eq = strchr(args[0], '=');
            if (eq) {
                size_t namelen = eq - args[0];
                size_t vallen  = strlen(eq + 1);
                if (namelen > 0 && vallen > 0) {
                    char name[namelen+1];
                    strncpy(name, args[0], namelen);
                    name[namelen] = '\0';
                    char *value = eq + 1;
                    if (strchr(name, ' ') || strchr(value, ' ')) {
                        fprintf(stderr, "Invalid command\n");
                    }
                    else if (set_var(name, value) < 0) {
                        perror("setting variable");
                    }
                    free(args);
                    continue;
                }
                fprintf(stderr, "Invalid command\n");
                free(args);
                continue;
            }
        }

        /* ── 3b) Variable expansion ($foo → its value, or empty if unset) ── */
        for (size_t j = 0; j < argcnt; j++) {
            char *expanded = expand_vars_in_str(args[j]);
            if (!expanded) {
                perror("expand_vars_in_str");
                expanded = strdup("");
                if (!expanded) {
                    expanded = (char *)malloc(1);
                    if (expanded) expanded[0] = '\0';
                }
            }
            args[j] = expanded;
        }

        /* 7) Built‐ins */
        if (strcmp(args[0], "exit") == 0) {
            printf("Good Bye\n");
            fflush(stdout);
            free(args);
            free(line);
            free_vars();
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
                char errbuf[512];
                int n = snprintf(errbuf, sizeof(errbuf),
                                 "%s: command not found\n", args[0]);
                if (n > 0)
                    write(STDOUT_FILENO, errbuf, (size_t)n);
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

        /* 9) Clean up this iteration */
        free(args);
    }

    /* EOF cleanup */
    free(line);
    free_vars();
    return last_status;
}
