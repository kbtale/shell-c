#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// JSON parser for CoinGecko simple price response: {"id":{"usd":price}}
static double parse_price_json(const char* json, const char* coin_id) {
    char search_str[256];
    snprintf(search_str, sizeof(search_str), "\"%s\"", coin_id);
    
    char* coin_ptr = strstr(json, search_str);
    if (!coin_ptr) return -1.0;
    
    char* usd_ptr = strstr(coin_ptr, "\"usd\"");
    if (!usd_ptr) return -1.0;
    
    char* colon_ptr = strchr(usd_ptr, ':');
    if (!colon_ptr) return -1.0;
    
    return atof(colon_ptr + 1);
}

int builtin_price(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: price <symbol> (e.g., price bitcoin)\n");
        return 1;
    }

    char url[512];
    snprintf(url, sizeof(url), "https://api.coingecko.com/api/v3/simple/price?ids=%s&vs_currencies=usd", args[1]);

    char* json = fetch_url_content(url);
    if (!json) {
        printf("Error: Could not fetch price data.\n");
        return 1;
    }

    double price = parse_price_json(json, args[1]);
    if (price < 0) {
        printf("Error: Coin '%s' not found or invalid response.\n", args[1]);
    } else {
        printf("\n  \033[1;32m%s\033[0m: $%.2f USD\n\n", args[1], price);
    }

    free(json);
    return 0;
}
