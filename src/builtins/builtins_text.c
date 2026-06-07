#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int builtin_grep(char **args, int arg_count) {
    if (arg_count < 3) {
        printf("Usage: grep <pattern> <file>\n");
        return 0;
    }
    FILE *fp = fopen(args[2], "r");
    if (!fp) {
        printf("grep: %s: No such file\n", args[2]);
        return 0;
    }
    char line[4096];
    int match_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, args[1])) {
            printf("\033[1;32m%s\033[0m", line);
            match_count++;
        }
    }
    fclose(fp);
    if (match_count == 0) printf("  No matches found.\n");
    return 0;
}

int builtin_wc(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: wc <file>\n");
        return 0;
    }
    FILE *fp = fopen(args[1], "r");
    if (!fp) {
        printf("wc: %s: No such file\n", args[1]);
        return 0;
    }
    int c;
    int lines = 0, words = 0, chars = 0;
    int in_word = 0;
    while ((c = fgetc(fp)) != EOF) {
        chars++;
        if (c == '\n') lines++;
        if (isspace(c)) in_word = 0;
        else if (!in_word) { in_word = 1; words++; }
    }
    fclose(fp);
    printf("  %d %d %d %s\n", lines, words, chars, args[1]);
    return 0;
}

int builtin_cal(char **args, int arg_count) {
    int year = 2025, month = 1;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (arg_count >= 2) month = atoi(args[1]);
    else month = tm_info->tm_mon + 1;
    if (arg_count >= 3) year = atoi(args[2]);
    else year = tm_info->tm_year + 1900;
    if (month < 1 || month > 12) { printf("cal: invalid month\n"); return 0; }

    static const char *months[] = {"January","February","March","April","May","June",
        "July","August","September","October","November","December"};
    static const int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int days = days_in_month[month - 1];
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) days = 29;

    struct tm first_day = {0};
    first_day.tm_year = year - 1900;
    first_day.tm_mon = month - 1;
    first_day.tm_mday = 1;
    mktime(&first_day);
    int start_dow = first_day.tm_wday;

    printf("\n\033[1;33m    %s %d\033[0m\n", months[month - 1], year);
    printf(" Su Mo Tu We Th Fr Sa\n");
    for (int i = 0; i < start_dow; i++) printf("   ");
    for (int d = 1; d <= days; d++) {
        printf("\033[1;32m%3d\033[0m", d);
        if ((start_dow + d) % 7 == 0) printf("\n");
    }
    if ((start_dow + days) % 7 != 0) printf("\n");
    printf("\n");
    return 0;
}

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int builtin_base64(char **args, int arg_count) {
    if (arg_count < 3 || (strcmp(args[1], "encode") != 0 && strcmp(args[1], "decode") != 0)) {
        printf("Usage: base64 <encode|decode> <text>\n");
        return 0;
    }
    if (strcmp(args[1], "encode") == 0) {
        const unsigned char *data = (unsigned char *)args[2];
        size_t len = strlen(args[2]);
        printf("  ");
        for (size_t i = 0; i < len; i += 3) {
            unsigned char a = data[i];
            unsigned char b = (i + 1 < len) ? data[i + 1] : 0;
            unsigned char c = (i + 2 < len) ? data[i + 2] : 0;
            printf("%c", b64_table[a >> 2]);
            printf("%c", b64_table[((a & 0x03) << 4) | (b >> 4)]);
            printf("%c", (i + 1 < len) ? b64_table[((b & 0x0f) << 2) | (c >> 6)] : '=');
            printf("%c", (i + 2 < len) ? b64_table[c & 0x3f] : '=');
        }
        printf("\n");
    } else {
        printf("  Base64 decode not yet implemented.\n");
    }
    return 0;
}

int builtin_rot13(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: rot13 <text>\n");
        return 0;
    }
    printf("  ");
    for (int i = 1; i < arg_count; i++) {
        for (char *c = args[i]; *c; c++) {
            if (*c >= 'a' && *c <= 'z') printf("%c", 'a' + (*c - 'a' + 13) % 26);
            else if (*c >= 'A' && *c <= 'Z') printf("%c", 'A' + (*c - 'A' + 13) % 26);
            else printf("%c", *c);
        }
        if (i < arg_count - 1) printf(" ");
    }
    printf("\n");
    return 0;
}
