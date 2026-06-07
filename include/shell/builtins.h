#ifndef SHELL_BUILTINS_H
#define SHELL_BUILTINS_H
#include "shell/common.h"

int is_builtin(const char *command);
int execute_builtin(char **args, int arg_count, int *exit_signal);

int builtin_echo(char **args, int arg_count);
int builtin_pwd(void);
int builtin_cd(char **args);
int builtin_type(char **args);
int builtin_ping(char **args);
int builtin_read(char **args);
int builtin_touch(char **args);
int builtin_weather(char **args);
int builtin_hexdump(char **args);
int builtin_bindump(char **args);
int builtin_mx(char **args);
int builtin_history(char **args);
int builtin_ls(char **args);
int builtin_banner(char **args);
int builtin_cshell(void);
int builtin_help(void);
int builtin_clear(void);
int builtin_whoami(void);
int builtin_ip(void);
int builtin_matrix(char **args, int arg_count);

#endif
