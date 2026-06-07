#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int builtin_calc(char **args, int arg_count) {
    if (arg_count < 4) {
        printf("Usage: calc <num> <op> <num> (e.g., calc 5 + 3)\n");
        printf("  Operators: + - * / %% ^\n");
        return 0;
    }
    double a = atof(args[1]);
    double b = atof(args[3]);
    double result = 0;
    char *op = args[2];

    if (strcmp(op, "+") == 0) result = a + b;
    else if (strcmp(op, "-") == 0) result = a - b;
    else if (strcmp(op, "*") == 0) result = a * b;
    else if (strcmp(op, "/") == 0) { if (b == 0) { printf("  Error: Division by zero\n"); return 0; } result = a / b; }
    else if (strcmp(op, "%") == 0) result = (long long)a % (long long)b;
    else if (strcmp(op, "^") == 0) result = pow(a, b);
    else { printf("  Unknown operator: %s\n", op); return 0; }

    printf("\n  \033[1;32m%.10g %s %.10g = %.10g\033[0m\n\n", a, op, b, result);
    return 0;
}

static unsigned int djb2(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

int builtin_hash(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: hash <text>\n");
        return 0;
    }
    unsigned int h = djb2(args[1]);
    printf("\n  \033[1;33mHash (djb2):\033[0m %08x\n\n", h);
    return 0;
}

int builtin_genpw(char **args, int arg_count) {
    int length = 16;
    if (arg_count >= 2) length = atoi(args[1]);
    if (length < 4) length = 4;
    if (length > 128) length = 128;

    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    printf("\n  \033[1;32m");
    srand(time(NULL) ^ getpid());
    for (int i = 0; i < length; i++) printf("%c", charset[rand() % (sizeof(charset) - 1)]);
    printf("\033[0m\n\n");
    return 0;
}

int builtin_uuid() {
    srand(time(NULL) ^ getpid());
    printf("\n  \033[1;32m");
    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) printf("-");
        else if (i == 14) printf("4");
        else if (i == 19) printf("%x", 8 + rand() % 4);
        else printf("%x", rand() % 16);
    }
    printf("\033[0m\n\n");
    return 0;
}

int builtin_ascii(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: ascii <char>\n");
        return 0;
    }
    printf("\n  \033[1;33mASCII Table\033[0m\n");
    if (strlen(args[1]) == 1) {
        unsigned char c = args[1][0];
        printf("  Char: '%c'  Dec: %d  Hex: 0x%02X  Oct: 0%o\n\n", c, c, c, c);
    } else {
        printf("  Dec  Hex  Char\n");
        for (int i = 32; i < 127; i++) {
            printf("  %3d  0x%02X  %c\n", i, i, (char)i);
        }
        printf("\n");
    }
    return 0;
}

int builtin_timer(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: timer <seconds>\n");
        return 0;
    }
    int seconds = atoi(args[1]);
    if (seconds <= 0) { printf("timer: Invalid duration\n"); return 0; }
    printf("\n  \033[1;33mTimer started: %d seconds\033[0m\n", seconds);
    for (int i = seconds; i > 0; i--) {
        printf("\r  \033[1;32m[%d]\033[0m", i);
        fflush(stdout);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
    printf("\r  \033[1;31m[TIME'S UP!]\033[0m    \n\n");
    return 0;
}
