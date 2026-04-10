#ifdef _WIN32
#include "shell/platform.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

char history_storage[MAX_HISTORY][MAX_CMD_LEN];
int history_count = 0;

void usleep(int microseconds) {
    Sleep(microseconds / 1000); 
}

void add_to_history_windows(const char *cmd) {
    if (strlen(cmd) == 0) return;
    
    // Don't add if it matches the previous command
    if (history_count > 0 && strcmp(history_storage[history_count-1], cmd) == 0) return;

    if (history_count < MAX_HISTORY) {
        strcpy(history_storage[history_count++], cmd);
    } else {
        // Shift everything up if full
        for (int i = 1; i < MAX_HISTORY; i++) {
            strcpy(history_storage[i-1], history_storage[i]);
        }
        strcpy(history_storage[MAX_HISTORY-1], cmd);
    }
}

void get_input_windows(char *buffer, int size) {
    int pos = 0;
    int view_index = history_count; // Start at the "new" empty line
    char c;
    
    // Clear buffer initially
    buffer[0] = '\0';
    printf("$ ");

    while (1) {
        c = _getch(); // Read key

        // --- ARROW KEYS (Special 2-byte codes: 224 or 0, then the code) ---
        if (c == -32 || c == 224) { 
            c = _getch(); // Get the actual code

            // UP ARROW (72)
            if (c == 72) { 
                if (view_index > 0) {
                    view_index--;
                    
                    // Clear current line on screen
                    while (pos > 0) { printf("\b \b"); pos--; }
                    
                    // Copy history to buffer
                    strcpy(buffer, history_storage[view_index]);
                    pos = strlen(buffer);
                    printf("%s", buffer);
                }
            }
            // DOWN ARROW (80)
            else if (c == 80) {
                if (view_index < history_count) {
                    view_index++;
                    
                    // Clear current line
                    while (pos > 0) { printf("\b \b"); pos--; }

                    if (view_index == history_count) {
                        // We are back at the empty new line
                        buffer[0] = '\0';
                    } else {
                        // Load next history item
                        strcpy(buffer, history_storage[view_index]);
                    }
                    pos = strlen(buffer);
                    printf("%s", buffer);
                }
            }
            continue;
        }

        // Handle Enter
        if (c == '\r' || c == '\n') {
            printf("\n");
            buffer[pos] = '\0';
            add_to_history_windows(buffer); 
            break;
        }

        // Handle Backspace
        if (c == 8) { 
            if (pos > 0) {
                pos--;
                printf("\b \b"); 
            }
            continue;
        }

        // Handle Normal Char
        if (pos < size - 1 && c >= 32 && c <= 126) {
            buffer[pos++] = c;
            printf("%c", c);
        }
    }
}
#endif
