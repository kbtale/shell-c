#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>

int builtin_cat(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: cat <file>\n");
        return 0;
    }
    FILE *fp = fopen(args[1], "r");
    if (!fp) {
        printf("cat: %s: No such file\n", args[1]);
        return 0;
    }
    char line[4096];
    while (fgets(line, sizeof(line), fp)) printf("%s", line);
    fclose(fp);
    return 0;
}
