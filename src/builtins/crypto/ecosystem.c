#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_exchanges(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: exchanges <coin-id> (e.g., exchanges bitcoin)\n");
        return 1;
    }

    const char* coin = args[1];
    char url[256];
    snprintf(url, sizeof(url), "https://api.coingecko.com/api/v3/coins/%s/tickers?order=volume_desc", coin);

    char* json = fetch_url_content(url);
    if (!json) {
        printf("Error: Could not fetch exchange data. Check the coin ID.\n");
        return 1;
    }

    printf("\n\033[1;33mTop Markets for %s\033[0m\n", coin);
    printf("  %-15s %-12s %-15s %-12s\n", "Exchange", "Pair", "Price", "24h Volume");

    char* pos = strstr(json, "\"tickers\":[");
    if (!pos) {
        printf("  No market data found.\n");
        free(json);
        return 0;
    }

    int count = 0;
    while (count < 8) {
        pos = strstr(pos, "{\"base\":");
        if (!pos) break;

        char market[64] = {0};
        char base[16] = {0};
        char target[16] = {0};
        double last = 0, vol = 0;

        char* market_ptr = strstr(pos, "\"name\":\"");
        char* base_ptr = strstr(pos, "\"base\":\"");
        char* target_ptr = strstr(pos, "\"target\":\"");
        char* last_ptr = strstr(pos, "\"last\":");
        char* vol_ptr = strstr(pos, "\"volume\":");

        if (market_ptr && base_ptr && target_ptr && last_ptr && vol_ptr) {
            sscanf(market_ptr + 8, "%[^\"]", market);
            sscanf(base_ptr + 8, "%[^\"]", base);
            sscanf(target_ptr + 10, "%[^\"]", target);
            sscanf(last_ptr + 7, "%lf", &last);
            sscanf(vol_ptr + 9, "%lf", &vol);

            printf("  %-15s %s/%-7s $%14.2f %10.2fM\n", market, base, target, last, vol / 1e6);
            count++;
        }
        pos += 10;
    }

    printf("\n");
    free(json);
    return 0;
}

int builtin_news(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: news <symbol> (e.g., news btc)\n");
        return 1;
    }

    const char* symbol = args[1];
    char url[512];
    snprintf(url, sizeof(url), "https://api.rss2json.com/v1/api.json?rss_url=https://cointelegraph.com/rss/tag/%s", symbol);

    char* json = fetch_url_content(url);
    if (!json) {
        printf("Error: Could not fetch news. Try a more common symbol.\n");
        return 1;
    }

    printf("\n\033[1;33mLatest News: %s\033[0m\n", symbol);

    char* pos = strstr(json, "\"items\":[");
    if (!pos) {
        printf("  No recent headlines found for this asset.\n");
        free(json);
        return 0;
    }

    int count = 0;
    while (count < 5) {
        pos = strstr(pos, "{\"title\":");
        if (!pos) break;

        char title[256] = {0};
        char link[256] = {0};

        char* title_ptr = strstr(pos, "\"title\":\"");
        char* link_ptr = strstr(pos, "\"link\":\"");

        if (title_ptr && link_ptr) {
            sscanf(title_ptr + 9, "%255[^\"]", title);
            sscanf(link_ptr + 8, "%255[^\"]", link);
            
            printf("  - %s\n", title);
            printf("    \033[0;36m%s\033[0m\n\n", link);
            count++;
        }
        pos += 10;
    }

    free(json);
    return 0;
}
