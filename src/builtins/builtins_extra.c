#include "shell/builtins.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int builtin_ping(char **args) {
    char cmd[256];
    if (args[1] == NULL) {
        printf("\n\033[1;36m[ PINGING MAINFRAME (8.8.8.8) ]\033[0m\n");
        #ifdef _WIN32
            snprintf(cmd, sizeof(cmd), "ping -n 10 8.8.8.8");
        #else
            snprintf(cmd, sizeof(cmd), "ping -c 10 8.8.8.8");
        #endif
    } else {
        snprintf(cmd, sizeof(cmd), "ping %s", args[1]);
    }
    system(cmd);
    return 0;
}

int builtin_read(char **args) {
    if (args[1] == NULL) {
        printf("Usage: read <url>\n");
        return 0;
    }
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "curl -s -L \"https://r.jina.ai/%s\"", args[1]);
    printf("\n\033[1;36m[ DECODING STREAM FROM: %s ]\033[0m\n\n", args[1]);
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("Connection lost");
        return 0;
    }
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) > 1) printf("  %s", line);
    }
    printf("\n\n");
    pclose(fp);
    return 0;
}

int builtin_touch(char **args) {
    if (args[1] == NULL) {
        printf("Usage: touch <filename>\n");
        return 0;
    }
    FILE *fp = fopen(args[1], "a");
    if (fp) {
        fclose(fp);
        printf("File '%s' touched.\n", args[1]);
    } else {
        perror("touch");
    }
    return 0;
}

int builtin_weather(char **args) {
    char cmd[256];
    if (args[1] == NULL) {
        snprintf(cmd, sizeof(cmd), "curl -s \"wttr.in?0\" 2>&1");
    } else {
        snprintf(cmd, sizeof(cmd), "curl -s \"wttr.in/%s?0\" 2>&1", args[1]);
    }
    printf("\n\033[1;36m[ ACCESSING METEOROLOGICAL SATELLITE... ]\033[0m\n");
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("Weather system failure");
        return 0;
    }
    char buffer[2048];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[bytes_read] = '\0';
    pclose(fp);
    if (bytes_read == 0 || strstr(buffer, "Usage:") || strstr(buffer, "Follow @igor_chubin")) {
        printf("\n\033[1;31m[!] ERROR: LOCATION AUTO-DETECT FAILED.\033[0m\n");
        printf("    The satellite could not triangulate your IP address.\n");
        printf("    \033[1;33mUsage: weather <city>\033[0m\n\n");
    } else {
        printf("\n%s\n", buffer);
    }
    return 0;
}

int builtin_hexdump(char **args) {
    if (args[1] == NULL) {
        printf("Usage: hexdump <filename>\n");
        return 0;
    }
    FILE *fp = fopen(args[1], "rb");
    if (!fp) {
        perror("hexdump");
        return 0;
    }
    unsigned char buffer[16];
    size_t bytes_read;
    size_t offset = 0;
    printf("\033[1;32m");
    while ((bytes_read = fread(buffer, 1, 16, fp)) > 0) {
        printf("%08zX  ", offset);
        for (int i = 0; i < 16; i++) {
            if (i < (int)bytes_read) printf("%02X ", buffer[i]);
            else printf("   ");
            if (i == 7) printf(" ");
        }
        printf(" |");
        for (int i = 0; i < (int)bytes_read; i++) {
            if (isprint(buffer[i])) printf("%c", buffer[i]);
            else printf(".");
        }
        printf("|\n");
        offset += bytes_read;
    }
    printf("\033[0m");
    fclose(fp);
    return 0;
}

int builtin_bindump(char **args) {
    if (args[1] == NULL) {
        printf("Usage: bindump <filename>\n");
        return 0;
    }
    FILE *fp = fopen(args[1], "rb");
    if (!fp) {
        perror("bindump");
        return 0;
    }
    unsigned char buffer[8];
    size_t bytes_read;
    size_t offset = 0;
    printf("\033[1;32m");
    while ((bytes_read = fread(buffer, 1, 8, fp)) > 0) {
        printf("%08zX  ", offset);
        for (int i = 0; i < 8; i++) {
            if (i < (int)bytes_read) {
                unsigned char byte = buffer[i];
                for (int b = 7; b >= 0; b--) printf("%d", (byte >> b) & 1);
                printf(" ");
            } else printf("         ");
        }
        printf(" |");
        for (int i = 0; i < (int)bytes_read; i++) {
            if (isprint(buffer[i])) printf("%c", buffer[i]);
            else printf(".");
        }
        printf("|\n");
        offset += bytes_read;
    }
    printf("\033[0m");
    fclose(fp);
    return 0;
}

int builtin_mx(char **args) {
    if (args[1] == NULL) {
        printf("Usage: mx <domain>\n");
        return 0;
    }
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "nslookup -type=mx %s", args[1]);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("Failed to run nslookup");
        return 0;
    }
    char line[512];
    int found_records = 0;
    printf("\n\033[1;36m[ SCANNING MAIL SERVERS FOR: %s ]\033[0m\n", args[1]);
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "mail exchanger")) {
            line[strcspn(line, "\r\n")] = 0;
            printf("  %s\033[1;32m%s\033[0m\n", "-> ", line);
            found_records++;
        }
    }
    if (found_records == 0) printf("  [!] No MX records found or connection failed.\n");
    printf("\n");
    pclose(fp);
    return 0;
}
