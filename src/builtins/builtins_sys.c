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
