#include "shell/utils.h"

char *get_path(char *command) {
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
    char *dir = strtok(path_copy, PATH_DELIMITER);

    while (dir != NULL) {
        char full_path[1024];
        #ifdef _WIN32
            snprintf(full_path, sizeof(full_path), "%s\\%s", dir, command);
        #else
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        #endif

        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return strdup(full_path);
        }
        dir = strtok(NULL, PATH_DELIMITER);
    }
    free(path_copy);
    return NULL;
}
