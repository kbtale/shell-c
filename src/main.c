#include "shell/common.h"
#include "shell/ui.h"
#include "shell/platform.h"
#include "shell/core.h"

int main(int argc, char *argv[]) {
    enable_ansi_support();
    setbuf(stdout, NULL);
    print_start_screen(0);

    #ifndef _WIN32
    rl_attempted_completion_function = builtin_completion;
    char *histfile = getenv("HISTFILE");
    if (histfile) {
        FILE *fp = fopen(histfile, "r");
        if (fp) {
            char line[1024];
            while (fgets(line, sizeof(line), fp)) {
                line[strcspn(line, "\r\n")] = '\0';
                if (strlen(line) > 0) add_history(line);
            }
            fclose(fp);
        }
    }
    #endif

    while (1) {
        char input[1024];
        shell_get_input(input, sizeof(input));
        if (strlen(input) == 0) continue;

        char *args[64];
        int arg_count = 0;
        tokenize_input(input, args, &arg_count);

        if (args[0] == NULL) continue;

        handle_redirection_and_piping(args, arg_count);

        // Cleanup args
        for (int i = 0; i < arg_count; i++) {
            if (args[i]) free(args[i]);
        }
    }

    #ifndef _WIN32
    if (histfile) {
        FILE *fp = fopen(histfile, "w");
        if (fp) {
            HIST_ENTRY **the_list = history_list();
            if (the_list) {
                for (int i = 0; the_list[i]; i++) {
                    fprintf(fp, "%s\n", the_list[i]->line);
                }
            }
            fclose(fp);
        }
    }
    #endif

    return 0;
}