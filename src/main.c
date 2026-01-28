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
    char *args[64];
    int arg_count = 0;
    
    char token_buf[1024]; 
    int token_pos = 0;
    char quote_char = 0; // 0 = none, ' = single, " = double

    for (int i = 0; i < strlen(input); i++) {
        char c = input[i];

// --- Handle Backslash Escaping ---
        if (c == '\\') {
            int should_escape = 0;
            
            // Case 1: Outside quotes -> Always escape
            if (quote_char == 0) {
                should_escape = 1;
            }
            // Case 2: Inside Double Quotes -> Only escape \ and "
            else if (quote_char == '"') {
                if (i + 1 < strlen(input)) {
                    char next_c = input[i+1];
                    if (next_c == '"' || next_c == '\\' || next_c == '$' || next_c == '\n') {
                        should_escape = 1;
                    }
                }
            }

            // Case 3: Inside Single Quotes -> Never escape (Implicitly should_escape = 0)

            // Execute the escape if condition met
            if (should_escape) {
                 if (i + 1 < strlen(input)) {
                    i++; // Skip the backslash
                    c = input[i]; // Grab the character being escaped
                    token_buf[token_pos++] = c; // Add it literally
                    continue; 
                }
            }
            // If I don't escape (e.g. "\n" inside double quotes), it falls through. The backslash is added as a normal character.
        }
        // --------------------------------------

        // Check for quotes
        if (c == '\'' || c == '"') {
            if (quote_char == 0) {
                quote_char = c; // Start quoting
                continue;
            }
            if (quote_char == c) {
                quote_char = 0; // End quoting
                continue;
            }
            // If we are in ", ' is text
            // If we are in ', " is text
        }

        // Handle space (Only split if NOT in any quotes)
        if (c == ' ' && quote_char == 0) {
            if (token_pos > 0) {
                token_buf[token_pos] = '\0';
                args[arg_count++] = strdup(token_buf);
                token_pos = 0;
            }
        } else {
            token_buf[token_pos++] = c;
        }
    }

    // Add the final word (if any)
    if (token_pos > 0) {
        token_buf[token_pos] = '\0';
        args[arg_count++] = strdup(token_buf);
    }
    args[arg_count] = NULL; // Null-terminate the list

    if (args[0] == NULL) continue;
    
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
        char *path = args[1];
        
        // Handle the "~" alias
        if (path != NULL && strcmp(path, "~") == 0) {
            
            // for Linux/Tests
            path = getenv("HOME");
            
            // If that fails, Windows USERPROFILE
            if (path == NULL) {
                path = getenv("USERPROFILE");
            }

            if (path == NULL) {
                printf("cd: HOME not set\n");
                continue;
            }
        }

        // Directory change, 0 if true
        if (chdir(path) != 0) {
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

    // External Program Execution
    char *command_path = get_path(args[0]);

    // FALLBACK: If not found in PATH, check if it exists in the current directory
    if (command_path == NULL && access(args[0], X_OK) == 0) {
        command_path = strdup(args[0]);
    }

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
        for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    }

  }
  return 0;
}
