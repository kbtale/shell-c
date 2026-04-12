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

int builtin_nft(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: nft <id> (e.g., nft bored-ape-yacht-club)\n");
        return 1;
    }

    const char* collection = args[1];
    char url[256];
    snprintf(url, sizeof(url), "https://api.coingecko.com/api/v3/nfts/%s", collection);

    char* json = fetch_url_content(url);
    if (!json) {
        printf("Error: Could not fetch NFT data. Ensure you used the correct ID (slug).\n");
        return 1;
    }

    printf("\n\033[1;33mNFT Collection Status\033[0m\n");

    char name[64] = {0};
    char asset_platform[32] = {0};
    double floor_eth = 0;
    double vol_24h = 0;

    char* name_ptr = strstr(json, "\"name\":\"");
    char* plat_ptr = strstr(json, "\"asset_platform_id\":\"");
    char* floor_ptr = strstr(json, "\"floor_price\":{\"native_currency\":");
    char* vol_ptr = strstr(json, "\"volume_24h\":{\"native_currency\":");

    if (name_ptr) sscanf(name_ptr + 8, "%[^\"]", name);
    if (plat_ptr) sscanf(plat_ptr + 21, "%[^\"]", asset_platform);
    if (floor_ptr) sscanf(floor_ptr + 33, "%lf", &floor_eth);
    if (vol_ptr) sscanf(vol_ptr + 33, "%lf", &vol_24h);

    if (name[0]) {
        printf("  Collection: %s\n", name);
        printf("  Platform:   %s\n", asset_platform[0] ? asset_platform : "Ethereum");
        printf("  Floor:      \033[1;32m%.3f ETH\033[0m\n", floor_eth);
        printf("  24h Volume: %.2f ETH\n", vol_24h);
    } else {
        printf("  Error: Could not parse collection details.\n");
    }

    printf("\n");
    free(json);
    return 0;
}
