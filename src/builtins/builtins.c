#include "shell/builtins.h"
#include <string.h>

extern int builtin_echo(char **args, int arg_count);
extern int builtin_pwd();
extern int builtin_cd(char **args);
extern int builtin_type(char **args);
extern int builtin_ping(char **args);
extern int builtin_read(char **args);
extern int builtin_touch(char **args);
extern int builtin_weather(char **args);
extern int builtin_hexdump(char **args);
extern int builtin_bindump(char **args);
extern int builtin_mx(char **args);
extern int builtin_history(char **args);
extern int builtin_ls(char **args);
extern int builtin_banner(char **args);
extern int builtin_cshell();
extern int builtin_clear();
extern int builtin_whoami();
extern int builtin_ip();
extern int builtin_price(char **args, int arg_count);
extern int builtin_convert(char **args, int arg_count);
extern int builtin_trending(char **args, int arg_count);
extern int builtin_feargreed();
extern int builtin_gas();
extern int builtin_addrinfo(char **args, int arg_count);
extern int builtin_balance(char **args, int arg_count);
extern int builtin_whale();
extern int builtin_tx(char **args, int arg_count);
extern int builtin_block(char **args, int arg_count);
extern int builtin_fees();
extern int builtin_defi(char **args, int arg_count);
extern int builtin_nft(char **args, int arg_count);
extern int builtin_exchanges(char **args, int arg_count);
extern int builtin_news(char **args, int arg_count);

static const char *builtins_list[] = {
    "echo", "exit", "type", "pwd", "cd", "history", "cshell", 
    "mx", "hexdump", "bindump", "weather", "help", "ls", 
    "clear", "cls", "ping", "read", "touch", "banner", "whoami", "ip", "price", "convert", "trending", "feargreed", "gas", "addrinfo", "balance", "whale", "tx", "block", "fees", "defi", "nft", "exchanges", "news"
};

int is_builtin(const char *command) {
    size_t num_builtins = sizeof(builtins_list) / sizeof(builtins_list[0]);
    for (size_t i = 0; i < num_builtins; i++) {
        if (strcmp(command, builtins_list[i]) == 0) return 1;
    }
    return 0;
}

int execute_builtin(char **args, int arg_count, int *exit_signal) {
    if (strcmp(args[0], "exit") == 0) {
        *exit_signal = 1;
        return 1;
    }
    if (strcmp(args[0], "echo") == 0) return builtin_echo(args, arg_count);
    if (strcmp(args[0], "pwd") == 0) return builtin_pwd();
    if (strcmp(args[0], "cd") == 0) return builtin_cd(args);
    if (strcmp(args[0], "type") == 0) return builtin_type(args);
    if (strcmp(args[0], "ping") == 0) return builtin_ping(args);
    if (strcmp(args[0], "read") == 0) return builtin_read(args);
    if (strcmp(args[0], "touch") == 0) return builtin_touch(args);
    if (strcmp(args[0], "weather") == 0) return builtin_weather(args);
    if (strcmp(args[0], "hexdump") == 0) return builtin_hexdump(args);
    if (strcmp(args[0], "bindump") == 0) return builtin_bindump(args);
    if (strcmp(args[0], "mx") == 0) return builtin_mx(args);
    if (strcmp(args[0], "history") == 0) return builtin_history(args);
    if (strcmp(args[0], "ls") == 0) return builtin_ls(args);
    if (strcmp(args[0], "banner") == 0) return builtin_banner(args);
    if (strcmp(args[0], "cshell") == 0) return builtin_cshell();
    if (strcmp(args[0], "help") == 0) { builtin_help(); return 0; }
    if (strcmp(args[0], "clear") == 0 || strcmp(args[0], "cls") == 0) return builtin_clear();
    if (strcmp(args[0], "whoami") == 0) return builtin_whoami();
    if (strcmp(args[0], "ip") == 0) return builtin_ip();
    if (strcmp(args[0], "price") == 0) return builtin_price(args, arg_count);
    if (strcmp(args[0], "convert") == 0) return builtin_convert(args, arg_count);
    if (strcmp(args[0], "trending") == 0) return builtin_trending(args, arg_count);
    if (strcmp(args[0], "feargreed") == 0) return builtin_feargreed();
    if (strcmp(args[0], "gas") == 0) return builtin_gas();
    if (strcmp(args[0], "addrinfo") == 0) return builtin_addrinfo(args, arg_count);
    if (strcmp(args[0], "balance") == 0) return builtin_balance(args, arg_count);
    if (strcmp(args[0], "whale") == 0) return builtin_whale();
    if (strcmp(args[0], "tx") == 0) return builtin_tx(args, arg_count);
    if (strcmp(args[0], "block") == 0) return builtin_block(args, arg_count);
    if (strcmp(args[0], "fees") == 0) return builtin_fees();
    if (strcmp(args[0], "defi") == 0) return builtin_defi(args, arg_count);
    if (strcmp(args[0], "nft") == 0) return builtin_nft(args, arg_count);
    if (strcmp(args[0], "exchanges") == 0) return builtin_exchanges(args, arg_count);
    if (strcmp(args[0], "news") == 0) return builtin_news(args, arg_count);
    
    return -1;
}
