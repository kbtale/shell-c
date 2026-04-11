#include "shell/ui.h"


const char *COLOR_CYAN = "\033[1;36m";
const char *COLOR_GREEN = "\033[1;32m";
const char *COLOR_MAGENTA = "\033[1;35m";
const char *COLOR_YELLOW = "\033[1;33m";
const char *COLOR_RED = "\033[1;31m";
const char *COLOR_BLUE = "\033[1;34m";
const char *COLOR_RESET = "\033[0m";

void enable_ansi_support() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; 
    SetConsoleMode(hOut, dwMode);
    SetConsoleOutputCP(65001);
    #endif
}
