#ifndef SHELL_COMMON_H
#define SHELL_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

#ifdef _WIN32
    #include <io.h>
    #include <process.h>
    #include <direct.h>
    #include <windows.h>
    #include <conio.h>

    #define access _access
    #ifdef X_OK
        #undef X_OK
    #endif
    #define X_OK 0
    #define PATH_DELIMITER ";"
    #define strdup _strdup
    #define getcwd _getcwd
    #define chdir _chdir
    
    void usleep(int microseconds);
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <readline/readline.h>
    #include <readline/history.h>

    #define PATH_DELIMITER ":"
#endif

#ifndef STDOUT_FILENO
    #define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
    #define STDERR_FILENO 2
#endif
#ifndef STDIN_FILENO
    #define STDIN_FILENO 0
#endif

#endif
