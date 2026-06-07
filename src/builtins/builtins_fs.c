#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>

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

int builtin_cp(char **args, int arg_count) {
    if (arg_count < 3) {
        printf("Usage: cp <src> <dst>\n");
        return 0;
    }
    FILE *src = fopen(args[1], "rb");
    if (!src) {
        printf("cp: %s: No such file\n", args[1]);
        return 0;
    }
    FILE *dst = fopen(args[2], "wb");
    if (!dst) {
        printf("cp: %s: Cannot create file\n", args[2]);
        fclose(src);
        return 0;
    }
    char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) fwrite(buf, 1, n, dst);
    fclose(src);
    fclose(dst);
    return 0;
}

int builtin_mv(char **args, int arg_count) {
    if (arg_count < 3) {
        printf("Usage: mv <src> <dst>\n");
        return 0;
    }
    if (rename(args[1], args[2]) != 0) {
        printf("mv: %s: Cannot move file\n", args[1]);
    }
    return 0;
}

int builtin_rm(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: rm <file>\n");
        return 0;
    }
    if (remove(args[1]) != 0) {
        printf("rm: %s: Cannot remove file\n", args[1]);
    }
    return 0;
}
