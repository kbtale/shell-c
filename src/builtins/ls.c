#include "shell/builtins.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

int builtin_ls(char **args) {
    int show_hidden = 0;
    if (args[1] != NULL && strcmp(args[1], "-a") == 0) {
        show_hidden = 1;
    }

    #ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("ls: Cannot access directory\n");
        return 0;
    }
    do {
        int is_hidden_dot = (findFileData.cFileName[0] == '.');
        if (!show_hidden && is_hidden_dot) continue;
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("\033[1;34m%s\033[0m  ", findFileData.cFileName);
        } else {
            printf("%s  ", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
    #else
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!show_hidden && dir->d_name[0] == '.') continue;
            if (stat(dir->d_name, &file_stat) == 0) {
                if (S_ISDIR(file_stat.st_mode)) {
                    printf("\033[1;34m%s\033[0m  ", dir->d_name);
                } else if (file_stat.st_mode & S_IXUSR) {
                    printf("\033[1;32m%s\033[0m  ", dir->d_name);
                } else {
                    printf("%s  ", dir->d_name);
                }
            } else {
                printf("%s  ", dir->d_name);
            }
        }
        closedir(d);
    } else {
        perror("ls");
    }
    #endif
    printf("\n");
    return 0;
}
