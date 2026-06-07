#include "shell/builtins.h"
#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_matrix(char **args, int arg_count) {
    printf("\033[H\033[J");
    srand(time(NULL));
    int rows = 30;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top;
    if (rows < 10) rows = 20;
#endif
    char chars[] = "0123456789ABCDEFabcdef@#$%&*+=<>{}[]|/\\:;!?-~";
    int cols[200] = {0};
    int drops[200] = {0};
    int green_cycles[200] = {0};
    int total_cols = 80;

#ifdef _WIN32
    total_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    if (total_cols < 20) total_cols = 80;
#endif

    for (int i = 0; i < total_cols; i++) {
        drops[i] = -(rand() % rows);
        green_cycles[i] = 3 + rand() % 8;
    }

    printf("\033[?25l");
    int frame = 0;
    while (frame < 300) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < total_cols; c++) {
                int y = r - drops[c];
                if (y >= 0 && y < rows) {
                    if (y == 0) printf("\033[1;37m%c\033[0m", chars[rand() % (sizeof(chars) - 1)]);
                    else if (y <= green_cycles[c]) printf("\033[1;32m%c\033[0m", chars[rand() % (sizeof(chars) - 1)]);
                    else printf(" ");
                } else printf(" ");
            }
            if (r < rows - 1) printf("\n");
        }
        for (int c = 0; c < total_cols; c++) {
            drops[c]++;
            if (drops[c] - green_cycles[c] > rows) {
                drops[c] = -(rand() % 10);
                green_cycles[c] = 3 + rand() % 8;
            }
        }
#ifdef _WIN32
        Sleep(60);
        if (frame < 299) printf("\033[H");
#else
        usleep(60000);
        if (frame < 299) printf("\033[H");
#endif
        frame++;
    }
    printf("\033[?25h\033[H\033[J");
    return 0;
}

int builtin_motd() {
    static const char *messages[] = {
        "[ THE NET IS VAST AND INFINITE ]",
        "[ FOLLOW THE WHITE RABBIT ]",
        "[ SYSTEM SHOCK IMMINENT ]",
        "[ GHOST DETECTED IN SHELL ]",
        "[ UPLOADING CONSCIOUSNESS... ]",
        "[ SECURITY LEVEL: PARANOID ]",
        "[ INITIALIZING NEURAL LINK... ]",
        "[ ESTABLISHING SECURE CONNECTION ]",
        "[ DECRYPTING ROOT ACCESS... ]",
        "[ BYPASSING MAINFRAME FIREWALL ]",
        "[ SCANNING PORTS... ]",
        "[ ALLOCATING MEMORY BLOCKS ]",
        "[ SYSTEM INTEGRITY: 99% ]",
        "[ SEARCHING FOR PROXIES... ]",
        "[ HIDING IP ADDRESS... ]",
        "[ SPOOFING MAC ADDRESS... ]",
        "[ ACCESSING GIBSON MAINFRAME ]",
        "[ REWRITING SYSTEM HISTORY... ]",
        "[ GLITCH IN THE MATRIX DETECTED ]",
        "[ TERMINAL ACCESS GRANTED ]",
        "[ EXECUTING STARTUP SCRIPT ]",
        "[ CHECKING BIOS INTEGRITY ]",
        "[ LOADING KERNEL MODULES... ]",
        "[ MOUNTING VIRTUAL DRIVES... ]",
        "[ ENCRYPTING FILE SYSTEM... ]",
        "[ DAEMON PROCESS STARTED ]",
        "[ LISTENING ON PORT 1337 ]",
        "[ PACKET LOSS DETECTED ]",
        "[ REBOOTING SIMULATION... ]",
        "[ COMPILING SOURCE CODE... ]",
        "[ SYNTAX ERROR IN REALITY ]",
        "[ 404: SOUL NOT FOUND ]",
        "[ CONNECTION ESTABLISHED ]"
    };
    int count = sizeof(messages) / sizeof(messages[0]);
    srand(time(NULL));
    int idx = rand() % count;
    printf("\n  \033[1;32m%s\033[0m\n\n", messages[idx]);
    return 0;
}

int builtin_skullsay(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: skullsay <text>\n");
        return 0;
    }
    char text[512] = {0};
    for (int i = 1; i < arg_count; i++) {
        strcat(text, args[i]);
        if (i < arg_count - 1) strcat(text, " ");
    }
    int len = strlen(text);
    printf("\n  ");
    for (int i = 0; i < len + 2; i++) printf("_");
    printf("\n");
    printf(" < %s >\n", text);
    printf("  ");
    for (int i = 0; i < len + 2; i++) printf("-");
    printf("\n");
    printf("       \\\n");
    printf("        \\\n");
    printf("         ,odO8888bo.\n");
    printf("      ,d8P'    `\"Y8b.\n");
    printf("    ,d88'  \033[1;31m(.)\033[0m  \033[1;31m(.)\033[0m `88b.\n");
    printf("   d8888b,  ,d8888888b\n");
    printf("   88888888888888888888\n");
    printf("   Y888888888888888888P\n");
    printf("    `\"Y888888888888P\"'\n");
    printf("        `\"\"Y888P\"\"'\n");
    printf("            \"\"\"\n\n");
    return 0;
}
