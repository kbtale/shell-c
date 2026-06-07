#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>

int builtin_ps(char **args, int arg_count) {
    printf("\n\033[1;33mActive Processes\033[0m\n");
#ifdef _WIN32
    system("tasklist | findstr /V \"svchost\"");
#else
    system("ps aux");
#endif
    printf("\n");
    return 0;
}

int builtin_uptime() {
#ifdef _WIN32
    system("wmic os get lastbootuptime");
#else
    system("uptime");
#endif
    return 0;
}

int builtin_date() {
    time_t now = time(NULL);
    printf("\n  \033[1;32m%s\033[0m\n\n", ctime(&now));
    return 0;
}

int builtin_env(char **args, int arg_count) {
    extern char **environ;
    for (char **env = environ; *env; env++) printf("%s\n", *env);
    return 0;
}

int builtin_uname(char **args, int arg_count) {
#ifdef _WIN32
    system("ver");
#else
    system("uname -a");
#endif
    return 0;
}

int builtin_df(char **args, int arg_count) {
    printf("\n\033[1;33mDisk Usage\033[0m\n");
#ifdef _WIN32
    system("wmic logicaldisk get size,freespace,caption");
#else
    system("df -h");
#endif
    printf("\n");
    return 0;
}

int builtin_du(char **args, int arg_count) {
    printf("\n\033[1;33mDirectory Size\033[0m\n");
#ifdef _WIN32
    system("dir /s | findstr \"File(s)\"");
#else
    system("du -sh .");
#endif
    printf("\n");
    return 0;
}
