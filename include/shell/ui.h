#ifndef SHELL_UI_H
#define SHELL_UI_H

#include "common.h"

extern const char *COLOR_CYAN;
extern const char *COLOR_GREEN;
extern const char *COLOR_MAGENTA;
extern const char *COLOR_YELLOW;
extern const char *COLOR_RED;
extern const char *COLOR_BLUE;
extern const char *COLOR_RESET;

void enable_ansi_support();
void print_start_screen(int show_all);

#endif
