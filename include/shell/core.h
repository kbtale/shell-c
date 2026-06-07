#ifndef SHELL_CORE_H
#define SHELL_CORE_H
#include "shell/common.h"

void tokenize_input(char *input, char **args, int *arg_count);
int execute_command(char **args, int arg_count);
void handle_redirection_and_piping(char **args, int arg_count);

#endif
