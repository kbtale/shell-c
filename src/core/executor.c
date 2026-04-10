#include "shell/core.h"
#include "shell/builtins.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/wait.h>
#endif

int execute_command(char **args, int arg_count) {
    int exit_signal = 0;
    int builtin_ret = execute_builtin(args, arg_count, &exit_signal);
    if (exit_signal) return 1;
    if (builtin_ret != -1) return 0;

    // External Program Execution
    char *command_path = get_path(args[0]);

    if (command_path == NULL) {
        if (access(args[0], 0) == 0) {
            char *relative_path = malloc(strlen(args[0]) + 3);
            sprintf(relative_path, "./%s", args[0]);
            command_path = relative_path;
        }
    }

    if (command_path != NULL) {
        #ifdef _WIN32
            _spawnv(_P_WAIT, command_path, (const char * const *)args);
        #else
            pid_t pid = fork();
            if (pid == 0) {
                execv(command_path, args);
                exit(1);
            } else {
                wait(NULL);
            }
        #endif
        free(command_path);
    } else {
        printf("%s: command not found\n", args[0]);
    }

    return 0;
}
