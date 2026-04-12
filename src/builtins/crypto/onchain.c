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
