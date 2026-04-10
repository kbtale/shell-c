#ifndef _WIN32
#include "shell/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void usleep(int microseconds) {
    // POSIX usleep is often in unistd.h, included via common.h
    // but we can provide a fallback if needed or just use the standard one.
}

char *command_generator(const char *text, int state) {
    static int list_index, len;
    static char *path_copy = NULL;
    static char *path_token = NULL;
    static DIR *dir = NULL;
    
    char *name;
    char *builtins[] = {"echo", "exit", "type", "pwd", "cd", "history", "ls", "ping", "help", "clear", "cshell", NULL};

    if (!state) {
        list_index = 0;
        len = strlen(text);
        
        if (path_copy) free(path_copy);
        char *env_path = getenv("PATH");
        path_copy = env_path ? strdup(env_path) : NULL;
        
        if (path_copy) {
            path_token = strtok(path_copy, PATH_DELIMITER);
        } else {
            path_token = NULL;
        }
        
        if (dir) { closedir(dir); dir = NULL; }
    }

    while ((name = builtins[list_index])) {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    if (!path_copy) return NULL;

    while (1) {
        if (!dir) {
            if (!path_token) break;
            dir = opendir(path_token);
            path_token = strtok(NULL, PATH_DELIMITER);
            if (!dir) continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, text, len) == 0) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
                return strdup(entry->d_name);
            }
        }
        closedir(dir);
        dir = NULL;
    }

    return NULL;
}

char **builtin_completion(const char *text, int start, int end) {
    if (start == 0) {
        rl_attempted_completion_over = 1; 
        return rl_completion_matches(text, command_generator);
    }
    return NULL;
}
#endif
