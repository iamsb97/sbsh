#ifndef HELPER_H
#define HELPER_H

#include <signal.h>

#define SBSH_INPUT_BUF 8

extern volatile sig_atomic_t sig_received;

void signal_handler();
void split_tokens(char *line, const char *delims, unsigned int *count, char **tokens);
void load_history(char *history_buffer[], int *curr_hist_index);
void replace_hist_buffer(char *history_buffer[], char *line, int *curr_index);
void write_history(char *history_buffer[], char write_mode, int *curr_index);
void free_hist_buffer(char *history_buffer[]);

#endif