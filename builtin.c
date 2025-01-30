#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

extern char *history_buffer[100];
extern int curr_hist_index;

int sbsh_exit(char **args) {
    return 0;
}

int sbsh_cd(char **args) {
    if (args[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
    }
    if (chdir(args[1]) == -1) {
        fprintf(stderr, "cd: %s\n", strerror(errno));
    }
    return 1;
}

int sbsh_history(char **args) {
    int null_cnt = 0;
    for (int i = 0; i < 100; i++) {
        int index = (i + curr_hist_index) % 100;
        if (history_buffer[index] != NULL) {
            fprintf(stdout, "%d  %s", i - null_cnt, history_buffer[index]);
        } else {
            null_cnt++;
        }
    }
    return 1;
}