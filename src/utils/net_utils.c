#include "shell/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* fetch_url_content(const char* url) {
    char command[2048];
    snprintf(command, sizeof(command), "curl -s \"%s\"", url);
    FILE* fp = popen(command, "r");
    if (fp == NULL) return NULL;
    char* response = malloc(8192);
    if (response == NULL) { pclose(fp); return NULL; }
    size_t total_read = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        if (total_read + len < 8191) {
            strcpy(response + total_read, buffer);
            total_read += len;
        }
    }
    response[total_read] = '\0';
    pclose(fp);
    return response;
}
