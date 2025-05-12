#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

/* === Allocation helpers that abort on NULL === */
static void *xmalloc(size_t sz) {
    void *p = malloc(sz);
    if (!p) { perror("malloc"); exit(1); }
    return p;
}
static void *xrealloc(void *ptr, size_t sz) {
    void *p = realloc(ptr, sz);
    if (!p) { perror("realloc"); exit(1); }
    return p;
}
static void *xcalloc(size_t nmemb, size_t size) {
    void *p = calloc(nmemb, size);
    if (!p) { perror("calloc"); exit(1); }
    return p;
}
static char *xstrdup(const char *s) {
    char *p = strdup(s);
    if (!p) { perror("strdup"); exit(1); }
    return p;
}

/* === Data structures & helpers for shell variables === */
typedef struct {
    char *name;
    char *value;
    int   exported;    // 0 = local only, 1 = also in environment
} ShellVar;

static ShellVar *vars     = NULL;
static size_t    vars_cnt = 0;
static size_t    vars_cap = 0;

// Validate that a shell‐variable name starts with letter/_ and continues with letter/digit/_ 
static int is_valid_name(const char *s) {
    if (!(isalpha((unsigned char)s[0]) || s[0]=='_')) return 0;
    for (const char *p = s+1; *p; ++p) {
        if (!(isalnum((unsigned char)*p) || *p=='_')) return 0;
    }
    return 1;
}

// Find index in vars[] or –1 if not found
static int find_var(const char *name) {
    for (size_t i = 0; i < vars_cnt; ++i) {
        if (strcmp(vars[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

// Set or update a shell variable. If export_flag=1, also call setenv(). 
static void shell_set_var(const char *name, const char *value, int export_flag) {
    int idx = find_var(name);
    if (idx >= 0) {
        // update existing
        free(vars[idx].value);
        vars[idx].value = xstrdup(value);
        if (export_flag || vars[idx].exported) {
            vars[idx].exported = 1;
            setenv(name, value, 1);
        }
    } else {
        // new entry
        if (vars_cnt == vars_cap) {
            size_t newcap = vars_cap ? vars_cap * 2 : 8;
            vars = (ShellVar*)xrealloc(vars, newcap * sizeof *vars);
            vars_cap = newcap;
        }
        vars[vars_cnt].name     = xstrdup(name);
        vars[vars_cnt].value    = xstrdup(value);
        vars[vars_cnt].exported = export_flag;
        if (export_flag) setenv(name, value, 1);
        vars_cnt++;
    }
}

// Lookup in shell vars first, then fallback to real environment
static const char *shell_get_var(const char *name) {
    int idx = find_var(name);
    if (idx >= 0) return vars[idx].value;
    return getenv(name);
}

static void free_all_vars(void) {
    for (size_t i = 0; i < vars_cnt; ++i) {
        free(vars[i].name);
        free(vars[i].value);
    }
    free(vars);
}

// Substitute any $VAR occurrences in 'arg' with their values
static char *substitute_vars(const char *arg) {
    size_t bufcap = strlen(arg) + 1;
    char *out     = (char*)xmalloc(bufcap);
    size_t o = 0;

    for (const char *p = arg; *p; ) {
        if (*p == '$') {
            const char *start = p + 1;
            size_t vn = 0;
            while (start[vn] && (isalnum((unsigned char)start[vn]) || start[vn]=='_'))
                vn++;
            if (vn == 0) {
                // literal '$'
                if (o + 1 >= bufcap) {
                    bufcap *= 2;
                    out = (char*)xrealloc(out, bufcap);
                }
                out[o++] = *p++;
            } else {
                char *name = (char*)xmalloc(vn + 1);
                memcpy(name, start, vn);
                name[vn] = '\0';
                const char *val = shell_get_var(name);
                free(name);
                if (!val) val = "";
                size_t vl = strlen(val);
                if (o + vl + 1 > bufcap) {
                    bufcap = o + vl + 1;
                    out = (char*)xrealloc(out, bufcap);
                }
                memcpy(out + o, val, vl);
                o += vl;
                p += vn + 1;
            }
        } else {
            if (o + 2 > bufcap) {
                bufcap *= 2;
                out = (char*)xrealloc(out, bufcap);
            }
            out[o++] = *p++;
        }
    }
    out[o] = '\0';
    return out;
}

/* === The Nano shell entry point === */
int nanoshell_main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int last_status = 0;

    for (;;) {
        /* 1) Prompt */
        if (printf("Nano Shell Prompt > ") < 0) {
            perror("printf");
            free_all_vars();
            free(line);
            return last_status;
        }
        fflush(stdout);

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
        if (len > 0 && line[len-1] == '\n') {
            line[--len] = '\0';
        }

        /* 4) Blank‐line? echo newline & re‐prompt */
        size_t i = 0;
        while (i < (size_t)len && line[i] == ' ') i++;
        if (i == (size_t)len) {
            putchar('\n');
            fflush(stdout);
            last_status = 0;
            continue;
        }

        /* 5) Echo newline */
        putchar('\n');
        fflush(stdout);

        /* 6) Tokenize */
        size_t argv_cap = 8, argcnt = 0;
        char **args = (char**)xmalloc(argv_cap * sizeof(char*));
        char *saveptr = NULL;
        char *tok = strtok_r(line, " ", &saveptr);
        while (tok) {
            if (argcnt + 1 >= argv_cap) {
                argv_cap *= 2;
                args = (char**)xrealloc(args, argv_cap * sizeof(char*));
            }
            args[argcnt++] = tok;
            tok = strtok_r(NULL, " ", &saveptr);
        }
        args[argcnt] = NULL;

        if (argcnt == 0) {
            free(args);
            continue;
        }

        /* 7) VAR=VALUE? */
        char *eq = strchr(args[0], '=');
        if (eq) {
            if (argcnt != 1 || eq == args[0] || eq[1] == '\0') {
                printf("Invalid command\n");
                last_status = 1;
            } else {
                size_t nlen = eq - args[0];
                char *name  = strndup(args[0], nlen);
                char *value = xstrdup(eq + 1);
                if (!is_valid_name(name)) {
                    printf("Invalid command\n");
                    last_status = 1;
                } else {
                    shell_set_var(name, value, 0);
                    last_status = 0;
                }
                free(name);
                free(value);
            }
            free(args);
            continue;
        }

        /* 8) Substitute $vars */
        char **subs = (char**)xcalloc(argcnt, sizeof(char*));
        for (size_t j = 0; j < argcnt; ++j) {
            if (strchr(args[j], '$')) {
                subs[j] = substitute_vars(args[j]);
                args[j] = subs[j];
            }
        }

        /* 9) Built‐ins */
        if (strcmp(args[0], "exit") == 0) {
            printf("Good Bye\n");
            fflush(stdout);
            free(args);
            for (size_t j = 0; j < argcnt; ++j) free(subs[j]);
            free(subs);
            free_all_vars();
            free(line);
            return last_status;
        }
        else if (strcmp(args[0], "cd") == 0) {
            const char *dir = (argcnt>1 ? args[1] : getenv("HOME"));
            if (!dir) dir = "/";
            if (chdir(dir) != 0) {
                fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
                last_status = 1;
            } else last_status = 0;
        }
        else if (strcmp(args[0], "pwd") == 0) {
            char cwd[4096];
            if (!getcwd(cwd, sizeof(cwd))) {
                perror("getcwd");
                last_status = 1;
            } else {
                printf("%s\n", cwd);
                last_status = 0;
            }
        }
        else if (strcmp(args[0], "echo") == 0) {
            for (size_t j = 1; j < argcnt; ++j) {
                if (j > 1) putchar(' ');
                fputs(args[j], stdout);
            }
            putchar('\n');
            last_status = 0;
        }
        else if (strcmp(args[0], "export") == 0) {
            if (argcnt != 2) {
                fprintf(stderr, "export: usage: export VAR\n");
                last_status = 1;
            }
            else if (!is_valid_name(args[1]) || find_var(args[1]) < 0) {
                fprintf(stderr, "export: %s: not found\n", args[1]);
                last_status = 1;
            } else {
                int idx = find_var(args[1]);
                shell_set_var(vars[idx].name, vars[idx].value, 1);
                last_status = 0;
            }
        }
        /* 10) External */
        else {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                last_status = 1;
            } else if (pid == 0) {
                execvp(args[0], args);
                char errbuf[512];
                int n = snprintf(errbuf, sizeof(errbuf),
                                 "%s: command not found\n", args[0]);
                if (n > 0) write(STDERR_FILENO, errbuf, (size_t)n);
                _exit(127);
            } else {
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

        /* 11) Per‐command cleanup */
        free(args);
        for (size_t j = 0; j < argcnt; ++j) free(subs[j]);
        free(subs);
    }

    /* 12) Final cleanup */
    free_all_vars();
    free(line);
    return last_status;
}

