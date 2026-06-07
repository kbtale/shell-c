#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include "common.h"

// Returns a mallocd string of the full path, or NULL if not found.
char *get_path(char *command);

void usleep(int microseconds);
char* fetch_url_content(const char* url);

#endif // SHELL_UTILS_H
