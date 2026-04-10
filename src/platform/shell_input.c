#include "shell/platform.h"
#include <string.h>

void shell_get_input(char *buffer, int size) {
    #ifdef _WIN32
        get_input_windows(buffer, size);
    #else
        char *input_ptr = readline("$ ");
        if (!input_ptr) {
            buffer[0] = '\0';
            return;
        }
        
        if (strlen(input_ptr) > 0) add_history(input_ptr);
        
        strncpy(buffer, input_ptr, size - 1);
        buffer[size - 1] = '\0';
        free(input_ptr);
    #endif
}
