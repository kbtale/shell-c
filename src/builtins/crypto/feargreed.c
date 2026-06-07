#include "shell/builtins.h"
#include "shell/common.h"
#include "shell/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_gauge(int value) {
    int bars = value / 4; // 25 total bars
    printf("  [");
    for (int i = 0; i < 25; i++) {
        if (i < bars) {
            if (value <= 25) printf("\033[1;31m#\033[0m"); // Red
            else if (value <= 45) printf("\033[1;33m#\033[0m"); // Yellow
            else if (value <= 55) printf("\033[1;37m#\033[0m"); // White
            else if (value <= 75) printf("\033[1;32m#\033[0m"); // Green
            else printf("\033[1;36m#\033[0m"); // Cyan
        } else {
            printf("-");
        }
    }
    printf("] %d/100\n", value);
}

int builtin_feargreed() {
    char url[] = "https://api.alternative.me/fng/";
    char* json = fetch_url_content(url);
    
    if (!json) {
        printf("Error: Could not fetch Fear & Greed data.\n");
        return 1;
    }

    char* val_ptr = strstr(json, "\"value\":");
    char* class_ptr = strstr(json, "\"value_classification\":");
    
    if (val_ptr && class_ptr) {
        int value = 0;
        char sentiment[32] = {0};
        
        sscanf(val_ptr + 9, "\"%d\"", &value);
        sscanf(class_ptr + 25, "\"%[^\"]\"", sentiment);
        
        printf("\n\033[1;33mCrypto Fear & Greed Index\033[0m\n");
        printf("  Sentiment: \033[1;32m%s\033[0m\n", sentiment);
        print_gauge(value);
        printf("\n");
    } else {
        printf("Error: Could not parse sentiment data.\n");
    }

    free(json);
    return 0;
}
