#ifndef SHELL_CORE_H
#define SHELL_CORE_H

#include "common.h"

// Parser
void tokenize_input(char *input, char **args, int *arg_count);

// Execution
int execute_command(char **args, int arg_count);

// Redirection & Piping
void handle_redirection_and_piping(char **args, int arg_count);

#endif // SHELL_CORE_H
