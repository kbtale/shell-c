#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_defi(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: defi <protocol> (e.g., defi uniswap)\n");
        return 1;
    }

    const char* protocol = args[1];
    char url[] = "https://api.llama.fi/protocols";
    char* json = fetch_url_content(url);

    if (!json) {
        printf("Error: Could not fetch DeFi data.\n");
        return 1;
    }

    printf("\n\033[1;33mDeFi Protocol Analysis\033[0m\n");
    
    char slug_pattern[128];
    snprintf(slug_pattern, sizeof(slug_pattern), "\"slug\":\"%s\"", protocol);
    
    char* protocol_ptr = strstr(json, slug_pattern);
    if (!protocol_ptr) {
        snprintf(slug_pattern, sizeof(slug_pattern), "\"name\":\"%s\"", protocol);
        protocol_ptr = strstr(json, slug_pattern);
    }

    if (protocol_ptr) {
        char name[64] = {0};
        char chain[32] = {0};
        double tvl = 0;

        char* name_ptr = strstr(protocol_ptr - 150, "\"name\":\"");
        char* tvl_ptr = strstr(protocol_ptr, "\"tvl\":");
        char* chain_ptr = strstr(protocol_ptr, "\"chain\":\"");

        if (name_ptr) sscanf(name_ptr + 8, "%[^\"]", name);
        if (tvl_ptr) sscanf(tvl_ptr + 6, "%lf", &tvl);
        if (chain_ptr) sscanf(chain_ptr + 9, "%[^\"]", chain);

        printf("  Protocol: %s\n", name[0] ? name : "Unknown");
        printf("  Network:  %s\n", chain[0] ? chain : "Multi-chain");
        printf("  TVL:      \033[1;32m$%.2fM USD\033[0m\n", tvl / 1e6);
    } else {
        printf("  Error: Protocol '%s' not found.\n", protocol);
    }

    printf("\n");
    free(json);
    return 0;
}
