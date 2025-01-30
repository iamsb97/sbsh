#ifndef BUILTIN_H
#define BUILTIN_H

int sbsh_exit(char **args);
int sbsh_cd(char **args);
int sbsh_history(char **args);

char *builtins[] = {
    "exit",
    "cd",
    "history"
};

int (*builtin_funcs[])(char **) = {
    sbsh_exit,
    sbsh_cd,
    sbsh_history
};

#endif