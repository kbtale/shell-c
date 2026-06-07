#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include "common.h"

char *get_path(char *command);
void usleep(int microseconds);
char* fetch_url_content(const char* url);

#endif
