#include "shell/builtins.h"
#include "shell/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int builtin_echo(char **args, int arg_count) {
    for (int i = 1; i < arg_count; i++) {
        printf("%s", args[i]);
        if (i < arg_count - 1) printf(" ");
    }
    printf("\n");
    return 0;
}

int builtin_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd failed");
    }
    return 0;
}

int builtin_cd(char **args) {
    char *path = args[1];
    if (path != NULL && strcmp(path, "~") == 0) {
        path = getenv("HOME");
        if (path == NULL) path = getenv("USERPROFILE");
        if (path == NULL) {
            printf("cd: HOME not set\n");
            return 0;
        }
    }
    if (chdir(path) != 0) {
        printf("cd: %s: No such file or directory\n", args[1]);
    }
    return 0;
}

int builtin_type(char **args) {
    if (args[1] == NULL) return 0;
    if (is_builtin(args[1])) {
        printf("%s is a shell builtin\n", args[1]);
    } else {
        char *path = get_path(args[1]); 
        if (path) {
            printf("%s is %s\n", args[1], path);
            free(path); 
        } else {
            printf("%s: not found\n", args[1]);
        }
    }
    return 0;
}
