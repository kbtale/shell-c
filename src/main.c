#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- CROSS-PLATFORM SETUP ---
#ifdef _WIN32
    // Windows-specific headers
    #include <io.h>       // For _access
    #include <process.h>  // For _spawnv (The Windows version of fork/exec)
    #include <direct.h>

    // Windows mappings to match Linux names
    #define access _access
    #define X_OK 0
    #define PATH_DELIMITER ";"
    #define strdup _strdup
    #define getcwd _getcwd
    #define chdir _chdir
#else
    // Linux/Mac-specific headers
    #include <unistd.h>   // For fork, execv, access
    #include <sys/wait.h> // For waitpid
    
    #define PATH_DELIMITER ":"
#endif
// ----------------------------

// --- HELPER: Find Executable in PATH ---
// Returns a mallocd string of the full path, or NULL if not found.
char *get_path(char *command) {
    // Check if command is a path (contains / or \) or exists locally
    if (strchr(command, '/') != NULL || strchr(command, '\\') != NULL) {
        if (access(command, X_OK) == 0) {
            return strdup(command);
        }
        return NULL;
    }

    // Search PATH environment variable
    char *path_env = getenv("PATH");
    if (path_env == NULL) return NULL;

    char *path_copy = strdup(path_env);
    char *dir = strtok(path_copy, PATH_DELIMITER);

    while (dir != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return strdup(full_path);
        }
        dir = strtok(NULL, PATH_DELIMITER);
    }
    free(path_copy);
    return NULL;
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  const char *builtins[] = {"echo", "exit", "type", "pwd", "cd"};
  size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

  while (1)
  {
    printf("$ ");
    
    char input[100];
    if (!fgets(input, sizeof(input), stdin)) break;
    input[strlen(input) - 1] = '\0'; // Remove newline character
    
    // Tokenize
    char *args[64]; // Array to hold the parts
    int arg_count = 0;
    
    char *token = strtok(input, " ");
    while (token != NULL && arg_count < 63) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // End the array for the system

    if (args[0] == NULL) continue; // Skip empty lines
    
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL && strcmp(args[1], "0") == 0) return 0;
        return 0;
    }

    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < arg_count; i++) {
            printf("%s", args[i]);
            // Only print a space if it's NOT the last word
            if (i < arg_count - 1) printf(" ");
        }
        printf("\n");
        continue;
    }

    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024]; // Buffer to store the path
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd failed");
        }
        continue;
    }

    if (strcmp(args[0], "cd") == 0) {
        // args[1] contains the path (e.g., "/usr/local/bin")
        // chdir returns 0 on success, -1 on failure
        if (chdir(args[1]) != 0) {
            printf("cd: %s: No such file or directory\n", args[1]);
        }
        continue;
    }

    if (strcmp(args[0], "type") == 0) {
      if (args[1] == NULL) continue;

      int found = 0;
      for (int i = 0; i < num_builtins; i++) {
        if (strcmp(args[1], builtins[i]) == 0) {
          printf("%s is a shell builtin\n", args[1]);
          found = 1;
          break;
        }
      }
      
      if (!found) {
         char *path = get_path(args[1]); 
         
         if (path) {
             printf("%s is %s\n", args[1], path);
             free(path); 
         } else {
             printf("%s: not found\n", args[1]);
         }
      }
      continue;
    }

    // If not a builtin, try to execute it
    char *command_path = get_path(args[0]);

    if (command_path != NULL) {
        #ifdef _WIN32
            // Windows execution
            _spawnv(_P_WAIT, command_path, args);
        #else
            // Linux execution
            pid_t pid = fork();
            if (pid == 0) {
                execv(command_path, args); // Run the program
                exit(1); // Only reached if execv fails
            } else {
                wait(NULL); // Wait for it to finish
            }
        #endif
        free(command_path);
    } else {
        printf("%s: command not found\n", args[0]);
    }

  }
  return 0;
}
