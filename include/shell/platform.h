#ifndef SHELL_PLATFORM_H
#define SHELL_PLATFORM_H

#include "common.h"

#ifdef _WIN32
    #define MAX_HISTORY 100
    #define MAX_CMD_LEN 1024
    extern char history_storage[MAX_HISTORY][MAX_CMD_LEN];
    extern int history_count;
    void add_to_history_windows(const char *cmd);
    void get_input_windows(char *buffer, int size);
#else
    char *command_generator(const char *text, int state);
    char **builtin_completion(const char *text, int start, int end);
#endif

void shell_get_input(char *buffer, int size);

#endif
