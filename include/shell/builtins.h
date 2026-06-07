#ifndef SHELL_BUILTINS_H
#define SHELL_BUILTINS_H

int execute_builtin(char **args, int arg_count, int *exit_signal);
int is_builtin(const char *command);

// Builtin implementations
int builtin_echo(char **args, int arg_count);
int builtin_exit();
int builtin_pwd();
int builtin_cd(char **args);
int builtin_type(char **args);
int builtin_help();
int builtin_price(char **args, int arg_count);
int builtin_convert(char **args, int arg_count);
int builtin_trending(char **args, int arg_count);
int builtin_feargreed();
int builtin_gas();
int builtin_addrinfo(char **args, int arg_count);
int builtin_balance(char **args, int arg_count);
int builtin_whale();
int builtin_tx(char **args, int arg_count);
int builtin_block(char **args, int arg_count);
int builtin_fees();
int builtin_defi(char **args, int arg_count);
int builtin_nft(char **args, int arg_count);
int builtin_nft(char **args, int arg_count);
int builtin_exchanges(char **args, int arg_count);
int builtin_news(char **args, int arg_count);
int builtin_halving();
int builtin_history(char **args);
int builtin_ls(char **args);
int builtin_cshell();
int builtin_clear();
int builtin_ping(char **args);
int builtin_read(char **args);
int builtin_touch(char **args);
int builtin_weather(char **args);
int builtin_hexdump(char **args);
int builtin_bindump(char **args);
int builtin_mx(char **args);
int builtin_banner(char **args);
int builtin_whoami();
int builtin_ip();
int builtin_cat(char **args, int arg_count);
int builtin_cp(char **args, int arg_count);
int builtin_mv(char **args, int arg_count);

#endif // SHELL_BUILTINS_H
