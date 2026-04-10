#include "shell/core.h"
#include <string.h>
#include <stdlib.h>

void tokenize_input(char *input, char **args, int *arg_count) {
    char token_buf[1024]; 
    int token_pos = 0;
    char quote_char = 0; // 0 = none, ' = single, " = double

    *arg_count = 0;

    for (int i = 0; i < (int)strlen(input); i++) {
        char c = input[i];

        // --- Handle Backslash Escaping ---
        if (c == '\\') {
            int should_escape = 0;
            
            if (quote_char == 0) {
                should_escape = 1;
            } else if (quote_char == '"') {
                if (i + 1 < (int)strlen(input)) {
                    char next_c = input[i+1];
                    if (next_c == '"' || next_c == '\\' || next_c == '$' || next_c == '\n') {
                        should_escape = 1;
                    }
                }
            }

            if (should_escape) {
                 if (i + 1 < (int)strlen(input)) {
                    i++;
                    c = input[i];
                    token_buf[token_pos++] = c;
                    continue; 
                }
            }
        }

        // Check for quotes
        if (c == '\'' || c == '"') {
            if (quote_char == 0) {
                quote_char = c;
                continue;
            }
            if (quote_char == c) {
                quote_char = 0;
                continue;
            }
        }

        // Handle space (Only split if NOT in any quotes)
        if (c == ' ' && quote_char == 0) {
            if (token_pos > 0) {
                token_buf[token_pos] = '\0';
                args[(*arg_count)++] = strdup(token_buf);
                token_pos = 0;
            }
        } else {
            token_buf[token_pos++] = c;
        }
    }

    if (token_pos > 0) {
        token_buf[token_pos] = '\0';
        args[(*arg_count)++] = strdup(token_buf);
    }
    args[*arg_count] = NULL;
}
