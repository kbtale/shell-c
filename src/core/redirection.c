#include "shell/core.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_redirection_and_piping(char **args, int arg_count) {
    // Check for pipe '|' symbol
    int has_pipe = 0;
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], "|") == 0) {
            has_pipe = 1;
            break;
        }
    }

    if (has_pipe) {
        #ifndef _WIN32
            char **commands[16];
            int num_cmds = 0;
            commands[num_cmds++] = args;

            for (int i = 0; i < arg_count; i++) {
                if (strcmp(args[i], "|") == 0) {
                    args[i] = NULL;
                    commands[num_cmds++] = &args[i + 1];
                }
            }

            int prev_pipe_read = -1;
            int pipefd[2];
            pid_t pids[16];

            for (int i = 0; i < num_cmds; i++) {
                if (i < num_cmds - 1) {
                    if (pipe(pipefd) == -1) { perror("pipe"); break; }
                }

                pids[i] = fork();
                if (pids[i] == 0) {
                    if (prev_pipe_read != -1) {
                        dup2(prev_pipe_read, STDIN_FILENO);
                        close(prev_pipe_read);
                    }
                    if (i < num_cmds - 1) {
                        dup2(pipefd[1], STDOUT_FILENO);
                        close(pipefd[1]);
                        close(pipefd[0]);
                    }
                    int sub_arg_count = 0;
                    while (commands[i][sub_arg_count] != NULL) sub_arg_count++;
                    execute_command(commands[i], sub_arg_count);
                    exit(0);
                }
                if (prev_pipe_read != -1) close(prev_pipe_read);
                if (i < num_cmds - 1) {
                    prev_pipe_read = pipefd[0];
                    close(pipefd[1]);
                }
            }
            for (int i = 0; i < num_cmds; i++) waitpid(pids[i], NULL, 0);
        #else
            printf("Pipelines not fully supported on Windows in this shell.\n");
        #endif
        return;
    }

    // --- REDIRECTION LOGIC ---
    char *stdout_file = NULL;
    char *stderr_file = NULL;
    int stdout_backup = -1;
    int stderr_backup = -1;
    int split_index = -1;
    int stdout_append = 0;
    int stderr_append = 0;
    
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0) {
             if (i + 1 < arg_count) {
                stdout_file = args[i+1];
                stdout_append = 1;
                if (split_index == -1 || i < split_index) split_index = i;
            }
        } else if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0) {
            if (i + 1 < arg_count) {
                stdout_file = args[i+1];
                stdout_append = 0;
                if (split_index == -1 || i < split_index) split_index = i;
            }
        } else if (strcmp(args[i], "2>>") == 0) {
            if (i + 1 < arg_count) {
                stderr_file = args[i+1];
                stderr_append = 1;
                if (split_index == -1 || i < split_index) split_index = i;
            }
        } else if (strcmp(args[i], "2>") == 0) {
            if (i + 1 < arg_count) {
                stderr_file = args[i+1];
                stderr_append = 0;
                if (split_index == -1 || i < split_index) split_index = i;
            }
        }
    }

    if (split_index != -1) args[split_index] = NULL;

    if (stdout_file != NULL) {
        stdout_backup = dup(STDOUT_FILENO); 
        int flags = O_WRONLY | O_CREAT;
        if (stdout_append) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        int fd = open(stdout_file, flags, 0644);
        if (fd < 0) { perror("open stdout"); return; }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if (stderr_file != NULL) {
        stderr_backup = dup(STDERR_FILENO); 
        int flags = O_WRONLY | O_CREAT;
        if (stderr_append) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        int fd = open(stderr_file, flags, 0644);
        if (fd < 0) {
            perror("open stderr");
            if (stdout_backup != -1) { dup2(stdout_backup, STDOUT_FILENO); close(stdout_backup); }
            return;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    execute_command(args, split_index != -1 ? split_index : arg_count);

    if (stdout_backup != -1) {
       fflush(stdout);
       dup2(stdout_backup, STDOUT_FILENO);
       close(stdout_backup);
    }
    if (stderr_backup != -1) {
       fflush(stderr);
       dup2(stderr_backup, STDERR_FILENO);
       close(stderr_backup);
    }
}
