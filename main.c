#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "main.h"
#include "builtin.h"
#include "helper.h"

char *history_buffer[100] = {NULL};
int curr_hist_index = 0;

int main (int argc, char **argv) {
    sbsh_loop();
    return EXIT_SUCCESS;
}

void sbsh_loop() {
    char *line;
    char **args;

    int status = 1;
    load_history(history_buffer, &curr_hist_index);

    while (status) {
        printf("sbsh$ ");

        INIT_INPUT(input);

        line = read_input();
        if (sig_received) {
            sig_received = 0;
            continue;
        }
        replace_hist_buffer(history_buffer, line, &curr_hist_index);
        parse_pipe(line, input);

        for (int i = 0; i < input->num_commands; i++) {
            if (input->is_piped) {
                if (pipe(input->fd[i]) < 0) {
                    fprintf(stderr, "Could not open pipe\n");
                    exit(EXIT_FAILURE);
                }
            }
            args = parse_command(input->commands_piped[i]);
            status = execute_command(args, input, i);
        }

        if (input->is_piped) {
            close(input->fd[0][0]);
            close(input->fd[1][0]);
            close(input->fd[0][1]);
            close(input->fd[1][1]);
        }

        free(args);
        free(line);
    }

    write_history(history_buffer, 'a', &curr_hist_index);
    free_hist_buffer(history_buffer);
}

char *read_input() {
    char *line = NULL;
    size_t buff_size = 0;

    signal(SIGINT, signal_handler);

    if (getline(&line, &buff_size, stdin) == -1) {
        free(line);
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

void parse_pipe(char *line, commands *input) {
    int curr_buf_size = SBSH_INPUT_BUF;

    input->commands_piped = (char **)malloc(curr_buf_size * sizeof(char *));
    input->num_commands = 0;

    split_tokens(line, SBSH_PIPE_DELIM, &(input->num_commands), input->commands_piped);

    if (input->num_commands > 1) {
        input->is_piped = true;
    } 
}

char **parse_command(char *command) {
    int curr_buf_size = SBSH_INPUT_BUF;

    char **args;
    args = (char **)malloc(curr_buf_size * sizeof(char *));
    unsigned int curr = 0;

    split_tokens(command, SBSH_SPLIT_DELIM, &curr, args);

    return args;
}

int execute_command(char **args, commands *input, int command_num) {
    char *command = args[0];

    if (command == NULL) {
        return 0;
    }

    int builtin_len = sizeof(builtins)/sizeof(char *);

    for (int i = 0; i < builtin_len; i++) {
        if (strcmp(builtins[i], command) == 0) { 
            return builtin_funcs[i](args);
        }
    }
    
    return execute_external_command(args, input, command_num);
}

int execute_external_command(char **args, commands *input, int command_num) {
    pid_t fp;
    int status;
    int curr_fd = command_num % 2;

    fp = fork();
    if (fp < 0) {
        perror("Forking process failed!");
        return 1;
    }
    
    if (fp == 0) {
        if (input->is_piped) {
            if (command_num == 0) {
                dup2(input->fd[curr_fd][1], STDOUT_FILENO);
            } else if (command_num == input->num_commands - 1) {
                dup2(input->fd[(curr_fd + 1) % 2][0], STDIN_FILENO);
            } else {
                dup2(input->fd[(curr_fd + 1) % 2][0], STDIN_FILENO);
                dup2(input->fd[curr_fd][1], STDOUT_FILENO);
            }

            close(input->fd[curr_fd][0]);
            close(input->fd[(curr_fd + 1) % 2][0]);

            close(input->fd[curr_fd][1]);
            close(input->fd[(curr_fd + 1) % 2][1]);
        }
        
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
            _exit(1);
        }
    } else {
        if (command_num > 0) {
            close(input->fd[(curr_fd + 1) % 2][0]);
            close(input->fd[(curr_fd + 1) % 2][1]);
        }
        
        waitpid(fp, &status, 0);
        if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            fprintf(stderr, "Execution terminated by signal %d: %s\n", signal, strsignal(signal));
        }
    }
    
    return 1;
}