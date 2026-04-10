#include "shell/builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <readline/history.h>
#endif

int builtin_history(char **args) {
    #ifndef _WIN32
        if (args[1] != NULL && strcmp(args[1], "-r") == 0) {
            if (args[2] == NULL) { printf("history: missing filename\n"); return 0; }
            FILE *fp = fopen(args[2], "r");
            if (fp) {
                char line[1024];
                while (fgets(line, sizeof(line), fp)) {
                    line[strcspn(line, "\r\n")] = '\0';
                    if (strlen(line) > 0) add_history(line);
                }
                fclose(fp);
            } else perror("history");
            return 0;
        } else if (args[1] != NULL && strcmp(args[1], "-w") == 0) {
            if (args[2] == NULL) { printf("history: missing filename\n"); return 0; }
            FILE *fp = fopen(args[2], "w");
            if (fp) {
                HIST_ENTRY **the_list = history_list();
                if (the_list) {
                    for (int i = 0; the_list[i]; i++) fprintf(fp, "%s\n", the_list[i]->line);
                }
                fclose(fp);
            } else perror("history");
            return 0;
        } else if (args[1] != NULL && strcmp(args[1], "-a") == 0) {
            if (args[2] == NULL) { printf("history: missing filename\n"); return 0; }
            FILE *fp = fopen(args[2], "a"); 
            if (fp) {
                HIST_ENTRY **the_list = history_list();
                if (the_list) {
                    static int history_write_index = 0;
                    int total_entries = 0;
                    while (the_list[total_entries]) total_entries++;
                    for (int i = history_write_index; i < total_entries; i++) fprintf(fp, "%s\n", the_list[i]->line);
                    history_write_index = total_entries;
                }
                fclose(fp);
            } else perror("history");
            return 0;
        }
        HIST_ENTRY **the_list = history_list();
        if (the_list) {
            int total_entries = 0;
            while (the_list[total_entries]) total_entries++;
            int start_index = 0;
            if (args[1] != NULL) {
                int limit = atoi(args[1]);
                if (limit > 0) {
                    start_index = total_entries - limit;
                    if (start_index < 0) start_index = 0;
                }
            }
            for (int i = start_index; the_list[i]; i++) printf("    %d  %s\n", i + 1, the_list[i]->line);
        }
    #else
        printf("History not supported on Windows mode.\n");
    #endif
    return 0;
}
