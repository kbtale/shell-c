#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell/utils.h"

#ifdef _WIN32
    #define DIR_SEPARATOR "\\"
#else
    #define DIR_SEPARATOR "/"
#endif

char *get_path(char *command) {
    if (command == NULL) return NULL;

    // Check if command is a path (contains / or \) or exists locally
    #ifdef _WIN32
        int is_path = (strchr(command, '/') != NULL || strchr(command, '\\') != NULL);
    #else
        int is_path = (strchr(command, '/') != NULL);
    #endif

    if (is_path) {
        if (access(command, 0) == 0) {
            return strdup(command);
        }
        return NULL;
    }

    // Search PATH environment variable
    char *path_env = getenv("PATH");
    if (path_env == NULL) return NULL;

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) return NULL;

    char *dir = strtok(path_copy, PATH_DELIMITER);
    while (dir != NULL) {
        // calculate exact memory needed for the combined path
        size_t needed = strlen(dir) + strlen(DIR_SEPARATOR) + strlen(command) + 1;
        char *full_path = malloc(needed);
        
        if (full_path == NULL) {
            free(path_copy);
            return NULL;
        }

        snprintf(full_path, needed, "%s%s%s", dir, DIR_SEPARATOR, command);

        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path; 
        }

        free(full_path);
        dir = strtok(NULL, PATH_DELIMITER);
    }

    free(path_copy);
    return NULL;
}
