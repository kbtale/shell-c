#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_trending(char **args, int arg_count) {
    int limit = 10;
    if (arg_count > 1) {
        limit = atoi(args[1]);
        if (limit <= 0) limit = 10;
    }

    char url[] = "https://api.coingecko.com/api/v3/search/trending";
    char* json = fetch_url_content(url);
    
    if (!json) {
        printf("Error: Could not fetch trending data.\n");
        return 1;
    }

    printf("\n\033[1;33mTop %d Trending Coins\033[0m\n", limit);
    
    char* pos = json;
    int count = 0;
    
    while (count < limit) {
        pos = strstr(pos, "\"item\":");
        if (!pos) break;
        
        char* name_ptr = strstr(pos, "\"name\":");
        char* symbol_ptr = strstr(pos, "\"symbol\":");
        char* rank_ptr = strstr(pos, "\"market_cap_rank\":");
        
        if (name_ptr && symbol_ptr && rank_ptr) {
            char name[64] = {0}, symbol[16] = {0};
            int rank = 0;
            
            sscanf(name_ptr + 8, "\"%[^\"]\"", name);
            sscanf(symbol_ptr + 10, "\"%[^\"]\"", symbol);
            sscanf(rank_ptr + 18, "%d", &rank);
            
            printf("  %d. \033[1;32m%-20s\033[0m (%s) [Rank: #%d]\n", count + 1, name, symbol, rank);
            count++;
        }
        pos += 7; 
    }

    if (count == 0) printf("  No trending data found.\n");
    printf("\n");

    free(json);
    return 0;
}
