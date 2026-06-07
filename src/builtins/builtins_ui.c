#include "shell/builtins.h"
#include "shell/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_banner(char **args) {
    if (args[1] == NULL) {
        printf("Usage: banner <text>\n");
        return 0;
    }
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "curl -s \"http://artii.herokuapp.com/make?text=%s&font=slant\"", args[1]);
    printf("\n\033[1;35m");
    fflush(stdout); 
    system(cmd);
    printf("\033[0m\n");
    return 0;
}

int builtin_cshell() {
    print_start_screen(1);
    return 0;
}

int builtin_help() {
    printf("\n\033[1;33m--- CSHELL MANUAL ---\033[0m\n");
    printf("  \033[1;32mascii [char]\033[0m: Show ASCII code or table\n");
    printf("  \033[1;32mbindump\033[0m    : View file in binary\n");
    printf("  \033[1;32mcalc <n> <op> <n>\033[0m: Simple calculator\n");
    printf("  \033[1;32mcd <dir>\033[0m   : Change directory\n");
    printf("  \033[1;32mclear/cls\033[0m  : Clear the terminal\n");
    printf("  \033[1;32mcshell\033[0m     : Show theme gallery\n");
    printf("  \033[1;32mecho <txt>\033[0m : Print text\n");
    printf("  \033[1;32mexit\033[0m       : Close shell\n");
    printf("  \033[1;32mgenpw [len]\033[0m: Generate random password\n");
    printf("  \033[1;32mhash <text>\033[0m: Compute hash of text\n");
    printf("  \033[1;32mhelp\033[0m       : Show this help\n");
    printf("  \033[1;32mhexdump\033[0m    : View file in hex\n");
    printf("  \033[1;32mhistory\033[0m    : Show or manage history\n");
    printf("  \033[1;32mip\033[0m         : Show network info\n");
    printf("  \033[1;32mls [-a]\033[0m    : List files (use -a for hidden)\n");
    printf("  \033[1;32mmx <dom>\033[0m   : Find mail servers for domain\n");
    printf("  \033[1;32mping [host]\033[0m: Ping a host (defaults to 8.8.8.8)\n");
    printf("  \033[1;32mpwd\033[0m        : Print working directory\n");
    printf("  \033[1;32mtype <cmd>\033[0m : Identify builtin or path\n");
    printf("  \033[1;32muuid\033[0m        : Generate a UUID v4\n");
    printf("  \033[1;32mweather\033[0m    : Get live weather report\n");
    printf("  \033[1;32mwhoami\033[0m     : Show current user\n\n");
    return 0;
}

int builtin_whoami() {
    char *user = getenv("USERNAME");
    if (user == NULL) user = getenv("USER");
    if (user == NULL) user = "ghost";
    printf("\n  \033[1;32mCurrent Operator:\033[0m %s\n\n", user);
    return 0;
}

int builtin_clear() {
    printf("\033[H\033[J");
    return 0;
}

int builtin_ip() {
    printf("\n\033[1;33m[ NETWORK INTERFACES ]\033[0m\n");
    #ifdef _WIN32
        system("ipconfig | findstr \"IPv4\"");
    #else
        system("hostname -I");
    #endif
    printf("\n");
    return 0;
}
