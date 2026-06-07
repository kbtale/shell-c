#ifndef SHELL_BUILTINS_H
#define SHELL_BUILTINS_H

int execute_builtin(char **args, int arg_count, int *exit_signal);
int is_builtin(const char *command);

int builtin_echo(char **args, int arg_count);
int builtin_exit();
int builtin_pwd();
int builtin_cd(char **args);
int builtin_type(char **args);
int builtin_help();
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
int builtin_grep(char **args, int arg_count);
int builtin_wc(char **args, int arg_count);
int builtin_cal(char **args, int arg_count);

#endif
