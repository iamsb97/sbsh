#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

void signal_handler() {
    sig_received = 1;
}

void split_tokens(char *line, const char *delims, unsigned int *count, char **tokens) {
    int curr_buf_size = SBSH_INPUT_BUF;
    
    char *token = strtok(line, delims);
    do {
        tokens[*count] = token;
        (*count)++;

        if (*count >= curr_buf_size) {
            curr_buf_size += SBSH_INPUT_BUF;
            tokens = realloc(tokens, curr_buf_size);
        }

        token = strtok(NULL, delims);
    } while (token != NULL);
}

void load_history(char *history_buffer[], int *curr_hist_index) {
    char *history_filename = malloc(strlen(getenv("HOME")) + strlen("/projects/sbsh/.sbsh_history"));
    strcpy(history_filename, getenv("HOME"));
    strcat(history_filename, "/projects/sbsh/.sbsh_history");
    
    FILE *file = fopen(history_filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Warning: Could not read history from .sbsh_history");
    }

    fseek(file, 0, SEEK_END);

    int lines = 0;
    long pos = ftell(file);
    while (pos >= 0 && lines <= 50) {
        fseek(file, pos, SEEK_SET);
        char ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
        pos--;
    }
    pos++;

    char *hist_line = NULL;
    size_t buffsize = 0;
    fseek(file, pos, SEEK_SET);
    while (*curr_hist_index < 100 && getline(&hist_line, &buffsize, file) != -1) {
        replace_hist_buffer(history_buffer, hist_line, curr_hist_index);
        free(hist_line);
        hist_line = NULL;
    }

    fclose(file);
    free(history_filename);
}

void replace_hist_buffer(char *history_buffer[], char *line, int *curr_index) {
    free(history_buffer[*curr_index]);

    history_buffer[*curr_index] = malloc(strlen(line) + 1);
    if (history_buffer[*curr_index] != NULL) {
        strcpy(history_buffer[*curr_index], line);
        *curr_index = *curr_index + 1;
        *curr_index %= 100;
    }
}

void write_history(char *history_buffer[], char write_mode, int *curr_index) {
    char *history_filename = malloc(strlen(getenv("HOME")) + strlen("/projects/sbsh/.sbsh_history"));
    strcpy(history_filename, getenv("HOME"));
    strcat(history_filename, "/projects/sbsh/.sbsh_history");

    FILE *file = fopen(history_filename, &write_mode);
    if (file == NULL) {
        fprintf(stderr, "Warning: Could not write history to .sbsh_history");
    }

    for (int i = 0; i < 100; i++) {
        int index = (i + *curr_index) % 100;
        if (history_buffer[index] != NULL) {
            fprintf(file, "%s", history_buffer[index]);
        }
    }

    fclose(file);
}

void free_hist_buffer(char *history_buffer[]) {
    for (int i = 0; i < 100; i++) {
        free(history_buffer[i]);
    }
}