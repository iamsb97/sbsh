#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

#define SBSH_INPUT_BUF 8
#define SBSH_SPLIT_DELIM " \t\n"
#define SBSH_PIPE_DELIM "|"

volatile sig_atomic_t sig_received = 0;

typedef struct commands {
    bool is_piped;
    unsigned int num_commands;
    char **commands_piped;
    int fd[2][2];
} commands;

#define INIT_INPUT(input) commands *input = &(commands){.is_piped = false, .num_commands = 0, .commands_piped = NULL}

void signal_handler();
void sbsh_loop();
char *read_input();
void parse_pipe(char *line, commands *input);
char **parse_command(char *command);
int execute_command(char **args, commands *input, int command_num);
int execute_external_command(char **args, commands *input, int command_num);

#endif
