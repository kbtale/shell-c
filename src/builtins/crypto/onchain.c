#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fetch_eth_gas() {
    char* json = fetch_url_content("https://api.etherscan.io/api?module=gastracker&action=gasoracle");
    if (!json) {
        printf("  Ethereum: Error fetching data\n");
        return;
    }

    char *low = strstr(json, "\"SafeGasPrice\":");
    char *med = strstr(json, "\"ProposeGasPrice\":");
    char *fast = strstr(json, "\"FastGasPrice\":");

    if (low && med && fast) {
        int l = 0, m = 0, f = 0;
        sscanf(low + 16, "\"%d\"", &l);
        sscanf(med + 19, "\"%d\"", &m);
        sscanf(fast + 16, "\"%d\"", &f);
        printf("  Ethereum: \033[1;32m%d\033[0m (Slow) | \033[1;33m%d\033[0m (Avg) | \033[1;31m%d\033[0m (Fast) Gwei\n", l, m, f);
    } else {
        printf("  Ethereum: Rate limited or invalid response\n");
    }
    free(json);
}

static void fetch_btc_gas() {
    char* json = fetch_url_content("https://mempool.space/api/v1/fees/recommended");
    if (!json) {
        printf("  Bitcoin: Error fetching data\n");
        return;
    }

    int fastest = 0, halfHour = 0, hour = 0;
    char *f_ptr = strstr(json, "\"fastestFee\":");
    char *h_ptr = strstr(json, "\"halfHourFee\":");
    char *hr_ptr = strstr(json, "\"hourFee\":");

    if (f_ptr && h_ptr && hr_ptr) {
        sscanf(f_ptr + 13, "%d", &fastest);
        sscanf(h_ptr + 14, "%d", &halfHour);
        sscanf(hr_ptr + 10, "%d", &hour);
        printf("  Bitcoin: \033[1;31m%d\033[0m (Fast) | \033[1;33m%d\033[0m (30m) | \033[1;32m%d\033[0m (1h) sat/vB\n", fastest, halfHour, hour);
    }
    free(json);
}

int builtin_gas() {
    printf("\n\033[1;33mNetwork Gas Tracker\033[0m\n");
    fetch_eth_gas();
    fetch_btc_gas();
    printf("\n");
    return 0;
}

static int is_base58(const char* s) {
    while (*s) {
        if (!((*s >= '1' && *s <= '9') || (*s >= 'A' && *s <= 'H') || 
              (*s >= 'J' && *s <= 'N') || (*s >= 'P' && *s <= 'Z') || 
              (*s >= 'a' && *s <= 'k') || (*s >= 'm' && *s <= 'z'))) return 0;
        s++;
    }
    return 1;
}

int builtin_addrinfo(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: addrinfo <address>\n");
        return 1;
    }

    const char* addr = args[1];
    int len = strlen(addr);

    printf("\n\033[1;33mAddress Analysis\033[0m\n");
    printf("  Target: %s\n", addr);

    if (len == 42 && addr[0] == '0' && addr[1] == 'x') {
        printf("  Network: \033[1;32mEthereum / EVM\033[0m\n");
        printf("  Type: EOA or Contract Address\n");
    } else if ((len >= 26 && len <= 35) && (addr[0] == '1' || addr[0] == '3')) {
        printf("  Network: \033[1;32mBitcoin\033[0m\n");
        printf("  Type: Legacy (P2PKH) or P2SH\n");
    } else if (len >= 42 && len <= 62 && strncmp(addr, "bc1", 3) == 0) {
        printf("  Network: \033[1;32mBitcoin (SegWit)\033[0m\n");
        printf("  Type: Bech32\n");
    } else if (len >= 32 && len <= 44 && is_base58(addr)) {
        printf("  Network: \033[1;32mSolana\033[0m\n");
        printf("  Type: Base58 Address\n");
    } else {
        printf("  Status: \033[1;31mUnknown or Invalid Format\033[0m\n");
    }
    printf("\n");
    return 0;
}

int builtin_balance(char **args, int arg_count) {
    if (arg_count < 3) {
        printf("Usage: balance <address> <chain> (e.g., balance 0x... eth)\n");
        return 1;
    }

    const char* addr = args[1];
    const char* chain = args[2];
    char url[512] = {0};

    printf("\n\033[1;33mWallet Balance\033[0m\n");

    if (strcmp(chain, "eth") == 0 || strcmp(chain, "ethereum") == 0) {
        snprintf(url, sizeof(url), "https://api.blockcypher.com/v1/eth/main/addrs/%s/balance", addr);
        char* json = fetch_url_content(url);
        if (json) {
            char* bal_ptr = strstr(json, "\"balance\":");
            if (bal_ptr) {
                long long wei = 0;
                sscanf(bal_ptr + 10, "%lld", &wei);
                printf("  Network: Ethereum\n");
                printf("  Balance: \033[1;32m%.6f ETH\033[0m\n", (double)wei / 1e18);
            } else {
                printf("  Error: Could not parse Ethereum balance.\n");
            }
            free(json);
        } else {
            printf("  Error: API request failed.\n");
        }
    } else if (strcmp(chain, "btc") == 0 || strcmp(chain, "bitcoin") == 0) {
        snprintf(url, sizeof(url), "https://blockchain.info/q/addressbalance/%s", addr);
        char* resp = fetch_url_content(url);
        if (resp) {
            long long sats = atoll(resp);
            printf("  Network: Bitcoin\n");
            printf("  Balance: \033[1;32m%.8f BTC\033[0m\n", (double)sats / 1e8);
            free(resp);
        } else {
            printf("  Error: API request failed.\n");
        }
    } else if (strcmp(chain, "ltc") == 0 || strcmp(chain, "litecoin") == 0 ||
               strcmp(chain, "doge") == 0 || strcmp(chain, "dogecoin") == 0 ||
               strcmp(chain, "dash") == 0) {
        
        const char* slug = strcmp(chain, "ltc") == 0 || strcmp(chain, "litecoin") == 0 ? "ltc" :
                          (strcmp(chain, "doge") == 0 || strcmp(chain, "dogecoin") == 0 ? "doge" : "dash");
        const char* name = strcmp(slug, "ltc") == 0 ? "Litecoin" : (strcmp(slug, "doge") == 0 ? "Dogecoin" : "Dash");

        snprintf(url, sizeof(url), "https://api.blockcypher.com/v1/%s/main/addrs/%s/balance", slug, addr);
        char* json = fetch_url_content(url);
        if (json) {
            char* bal_ptr = strstr(json, "\"balance\":");
            if (bal_ptr) {
                long long units = 0;
                sscanf(bal_ptr + 10, "%lld", &units);
                printf("  Network: %s\n", name);
                printf("  Balance: \033[1;32m%.8f %s\033[0m\n", (double)units / 1e8, slug);
            } else {
                printf("  Error: Could not parse %s balance.\n", name);
            }
            free(json);
        } else {
            printf("  Error: API request failed.\n");
        }
    } else {
        printf("  Error: Unsupported chain '%s'. Use 'eth', 'btc', 'ltc', 'doge', or 'dash'.\n", chain);
    }

    printf("\n");
    return 0;
}

int builtin_whale() {
    char url[] = "https://api.ethplorer.io/getTopTransfers?apiKey=freekey";
    char* json = fetch_url_content(url);
    
    if (!json) {
        printf("Error: Could not fetch whale data.\n");
        return 1;
    }

    printf("\n\033[1;33mRecent Whale Activity (Ethereum)\033[0m\n");
    
    char* pos = json;
    int count = 0;
    while (count < 5) {
        pos = strstr(pos, "{\"id\":");
        if (!pos) break;

        char* amount_ptr = strstr(pos, "\"amount\":");
        char* symbol_ptr = strstr(pos, "\"symbol\":");
        char* value_ptr = strstr(pos, "\"usdValue\":");

        if (amount_ptr && symbol_ptr && value_ptr) {
            double amount = 0, value = 0;
            char symbol[16] = {0};

            sscanf(amount_ptr + 9, "%lf", &amount);
            sscanf(symbol_ptr + 10, "\"%[^\"]\"", symbol);
            sscanf(value_ptr + 11, "%lf", &value);

            if (value > 500000) { 
                printf("  🐋 \033[1;32m%.2f %s\033[0m (~$%.2fM USD)\n", amount, symbol, value / 1e6);
                count++;
            }
        }
        pos += 5;
    }

    if (count == 0) printf("  No massive movements detected.\n");
    printf("\n");

    free(json);
    return 0;
}

int builtin_tx(char **args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: tx <hash> [chain] (e.g., tx 0x... eth)\n");
        return 1;
    }

    const char* hash = args[1];
    const char* chain = (arg_count > 2) ? args[2] : "eth";
    char url[512] = {0};

    printf("\n\033[1;33mTransaction Explorer\033[0m\n");
    printf("  Hash: %s\n", hash);

    if (strcmp(chain, "eth") == 0) {
        snprintf(url, sizeof(url), "https://api.blockcypher.com/v1/eth/main/txs/%s", hash);
        char* json = fetch_url_content(url);
        if (json) {
            char* val_ptr = strstr(json, "\"total\":");
            char* conf_ptr = strstr(json, "\"confirmations\":");
            if (val_ptr && conf_ptr) {
                long long total = 0;
                int confs = 0;
                sscanf(val_ptr + 8, "%lld", &total);
                sscanf(conf_ptr + 16, "%d", &confs);
                printf("  Network: Ethereum\n");
                printf("  Total Value: \033[1;32m%.6f ETH\033[0m\n", (double)total / 1e18);
                printf("  Confirmations: %d\n", confs);
            } else {
                printf("  Error: Transaction not found.\n");
            }
            free(json);
        }
    } else if (strcmp(chain, "btc") == 0) {
        snprintf(url, sizeof(url), "https://blockchain.info/rawtx/%s", hash);
        char* json = fetch_url_content(url);
        if (json) {
            char* val_ptr = strstr(json, "\"out\":"); 
            if (val_ptr) {
                printf("  Network: Bitcoin\n");
                printf("  Status: \033[1;32mConfirmed\033[0m\n");
                printf("  Details: blockchain.com/btc/tx/%s\n", hash);
            } else {
                printf("  Error: Transaction not found.\n");
            }
            free(json);
        }
    } else {
        printf("  Error: Chain '%s' not supported.\n", chain);
    }

    printf("\n");
    return 0;
}
