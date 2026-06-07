#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_http(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: http <url>\n");
        return 0;
    }
    printf("\n\033[1;33mHTTP Status Check\033[0m\n");
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -s -o /dev/null -w \"%%{http_code}\" -L \"%s\"", args[1]);
    FILE *fp = popen(cmd, "r");
    if (!fp) { printf("  Error: Could not connect\n"); return 0; }
    char code[16] = {0};
    fgets(code, sizeof(code), fp);
    pclose(fp);
    printf("  \033[1;32m%s\033[0m -> HTTP %s\n\n", args[1], code);
    return 0;
}

int builtin_geoip(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: geoip <ip>\n");
        return 0;
    }
    printf("\n\033[1;33mGeoIP Lookup\033[0m\n");
    char url[256];
    snprintf(url, sizeof(url), "http://ip-api.com/json/%s", args[1]);
    char *json = fetch_url_content(url);
    if (!json) { printf("  Error: Could not fetch data\n\n"); return 0; }
    char *city = strstr(json, "\"city\":");
    char *country = strstr(json, "\"country\":");
    char *isp = strstr(json, "\"isp\":");
    char *org = strstr(json, "\"org\":");
    if (city) {
        char city_str[64] = {0}, country_str[64] = {0}, isp_str[64] = {0}, org_str[64] = {0};
        sscanf(city + 8, "\"%63[^\"]\"", city_str);
        sscanf(country + 11, "\"%63[^\"]\"", country_str);
        sscanf(isp + 7, "\"%63[^\"]\"", isp_str);
        sscanf(org + 7, "\"%63[^\"]\"", org_str);
        printf("  IP: \033[1;32m%s\033[0m\n", args[1]);
        printf("  Location: %s, %s\n", city_str, country_str);
        printf("  ISP: %s\n", isp_str);
        printf("  Org: %s\n", org_str);
    } else {
        printf("  No data found for %s\n", args[1]);
    }
    printf("\n");
    free(json);
    return 0;
}

int builtin_port(char **args, int arg_count) {
    if (arg_count < 3) {
        printf("Usage: port <host> <port>\n");
        return 0;
    }
    printf("\n\033[1;33mPort Scanner\033[0m\n");
    printf("  Checking %s:%s ... ", args[1], args[2]);
    char cmd[256];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "powershell -c \"(Test-NetConnection %s -Port %s -WarningAction SilentlyContinue).TcpTestSucceeded\" 2>nul", args[1], args[2]);
#else
    snprintf(cmd, sizeof(cmd), "timeout 3 bash -c 'echo >/dev/tcp/%s/%s' 2>/dev/null && echo True || echo False", args[1], args[2]);
#endif
    FILE *fp = popen(cmd, "r");
    if (!fp) { printf("\n\n  Error\n"); return 0; }
    char result[32] = {0};
    fgets(result, sizeof(result), fp);
    pclose(fp);
    if (strstr(result, "True")) printf("\033[1;32mOPEN\033[0m\n");
    else printf("\033[1;31mCLOSED/FILTERED\033[0m\n");
    printf("\n");
    return 0;
}

int builtin_dig(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: dig <domain>\n");
        return 0;
    }
    printf("\n\033[1;33mDNS Lookup: %s\033[0m\n", args[1]);
    char cmd[256];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "nslookup %s", args[1]);
#else
    snprintf(cmd, sizeof(cmd), "dig +short %s", args[1]);
#endif
    FILE *fp = popen(cmd, "r");
    if (!fp) { printf("  Error\n\n"); return 0; }
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
#ifdef _WIN32
        if (strstr(line, "Address") && !strstr(line, "#")) { printf("  \033[1;32m-> %s\033[0m", line); found = 1; }
#else
        if (strlen(line) > 1) { printf("  \033[1;32m-> %s\033[0m", line); found = 1; }
#endif
    }
    pclose(fp);
    if (!found) printf("  No records found.\n");
    printf("\n");
    return 0;
}
