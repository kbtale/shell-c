#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double parse_conversion_rate(const char* json, const char* from_id, const char* to_id) {
    char search_from[256], search_to[256];
    snprintf(search_from, sizeof(search_from), "\"%s\"", from_id);
    snprintf(search_to, sizeof(search_to), "\"%s\"", to_id);
    
    char* from_ptr = strstr(json, search_from);
    if (!from_ptr) return -1.0;
    
    char* to_ptr = strstr(from_ptr, search_to);
    if (!to_ptr) return -1.0;
    
    char* colon_ptr = strchr(to_ptr, ':');
    if (!colon_ptr) return -1.0;
    
    return atof(colon_ptr + 1);
}

int builtin_convert(char **args, int arg_count) {
    if (arg_count < 4) {
        printf("Usage: convert <amount> <from> <to> (e.g., convert 1 bitcoin ethereum)\n");
        return 1;
    }

    double amount = atof(args[1]);
    char* from = args[2];
    char* to = args[3];

    char url[512];
    snprintf(url, sizeof(url), "https://api.coingecko.com/api/v3/simple/price?ids=%s&vs_currencies=%s", from, to);

    char* json = fetch_url_content(url);
    if (!json) {
        printf("Error: Could not fetch conversion data.\n");
        return 1;
    }

    double rate = parse_conversion_rate(json, from, to);
    if (rate < 0) {
        printf("Error: Assets not found or invalid conversion pair.\n");
    } else {
        double result = amount * rate;
        printf("\n  \033[1;32m%.4f %s\033[0m = \033[1;36m%.4f %s\033[0m (Rate: %.6f)\n\n", amount, from, result, to, rate);
    }

    free(json);
    return 0;
}
