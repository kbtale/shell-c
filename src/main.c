#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// Standard File Descriptor for generic OS
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

// Standard Error Definition
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
// ----------------------------------------

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
    // On Linux, only / indicates a path. On Windows, both / and \ do
    #ifdef _WIN32
        int is_path = (strchr(command, '/') != NULL || strchr(command, '\\') != NULL);
    #else
        int is_path = (strchr(command, '/') != NULL);
    #endif

    if (is_path) {
        if (access(command, 0) == 0) {
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

// --- EXECUTOR FUNCTION ---
// Handles the logic for all commands. Returns 1 if shell should exit, 0 otherwise.
int execute_command(char **args, int arg_count) {
    const char *builtins[] = {"echo", "exit", "type", "pwd", "cd"};
    size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL && strcmp(args[1], "0") == 0) return 1; // Exit signal
        return 1; // Exit signal
    }

    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < arg_count; i++) {
            printf("%s", args[i]);
            // Only print a space if it's NOT the last word
            if (i < arg_count - 1) printf(" ");
        }
        printf("\n");
        return 0;
    }

    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024]; // Buffer to store the path
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd failed");
        }
        return 0;
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
                return 0;
            }
        }

        // Directory change, 0 if true
        if (chdir(path) != 0) {
            printf("cd: %s: No such file or directory\n", args[1]);
        }
        return 0;
    }

    if (strcmp(args[0], "type") == 0) {
      if (args[1] == NULL) return 0;

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
      return 0;
    }

    // External Program Execution
    char *command_path = get_path(args[0]);

    // FALLBACK: If NOT in PATH, check if it exists in the current directory
    if (command_path == NULL) {
        // Use 0 (F_OK) to check existence. 
        if (access(args[0], 0) == 0) {
            // prepend "./" for execv to work on Linux
            // "myprog" -> "./myprog"
            char *relative_path = malloc(strlen(args[0]) + 3);
            sprintf(relative_path, "./%s", args[0]);
            command_path = relative_path;
        }
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
    }

    return 0;
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");
    
    char input[100];
    if (!fgets(input, sizeof(input), stdin)) break;
    input[strcspn(input, "\r\n")] = '\0'; // Remove newline character
    
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

    // --- REDIRECTION LOGIC ---
    char *stdout_file = NULL;
    char *stderr_file = NULL;
    int stdout_backup = -1;
    int stderr_backup = -1;
    int split_index = -1;
    int stdout_append = 0;
    int stderr_append = 0;
    
    // Scan for >, >>, 1>, 1>>, 2>, or 2>>
    for (int i = 0; i < arg_count; i++) {
        
        // Check for Append (>> or 1>>)
        if (strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0) {
             if (i + 1 < arg_count) {
                stdout_file = args[i+1];
                stdout_append = 1; // Enable append mode
                // Record the earliest redirection operator to cut the string later
                if (split_index == -1 || i < split_index) split_index = i;
            }
        }
        // Check for Standard Output Redirection (> or 1>)
        else if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0) {
            if (i + 1 < arg_count) {
                stdout_file = args[i+1];
                stdout_append = 0; // Disable append mode
                // Record the earliest redirection operator to cut the string later
                if (split_index == -1 || i < split_index) split_index = i;
            }
        }
        // Check for Standard Error Append (2>>)
        else if (strcmp(args[i], "2>>") == 0) {
            if (i + 1 < arg_count) {
                stderr_file = args[i+1];
                stderr_append = 1; // Enable append mode for stderr
                // Record the earliest redirection operator
                if (split_index == -1 || i < split_index) split_index = i;
            }
        }
        // Check for Standard Error Redirection (2>)
        else if (strcmp(args[i], "2>") == 0) {
            if (i + 1 < arg_count) {
                stderr_file = args[i+1];
                stderr_append = 0; // Disable append mode
                // Record the earliest redirection operator
                if (split_index == -1 || i < split_index) split_index = i;
            }
        }
    }

    // Truncate the args array so the command doesn't see filenames
    if (split_index != -1) {
        args[split_index] = NULL;
        arg_count = split_index;
    }

    // 1. Handle STDOUT redirection
    if (stdout_file != NULL) {
        // Save the current stdout (terminal) so we can restore it later
        stdout_backup = dup(STDOUT_FILENO); 

        // Set flags based on mode
        int flags = O_WRONLY | O_CREAT;
        if (stdout_append) {
            flags |= O_APPEND; // Append to file
        } else {
            flags |= O_TRUNC;  // Overwrite file
        }

        // Open the file (Create if missing, Truncate/Append, Write only)
        // 0644 gives read/write to owner, read to others
        int fd = open(stdout_file, flags, 0644);
        
        if (fd < 0) {
            perror("Failed to open stdout file");
            continue;
        }

        // Replace stdout (fd 1) with our file descriptor
        dup2(fd, STDOUT_FILENO);
        
        // Close the raw file descriptor (stdout now holds the reference)
        close(fd);
    }

    // Handle STDERR redirection
    if (stderr_file != NULL) {
        stderr_backup = dup(STDERR_FILENO); 

        // Set flags based on mode for stderr
        int flags = O_WRONLY | O_CREAT;
        if (stderr_append) {
            flags |= O_APPEND; // Append to file
        } else {
            flags |= O_TRUNC;  // Overwrite file
        }

        int fd = open(stderr_file, flags, 0644);
        
        if (fd < 0) {
            perror("Failed to open stderr file");
            // If stderr fails, restore stdout before continuing
            if (stdout_backup != -1) { dup2(stdout_backup, STDOUT_FILENO); close(stdout_backup); }
            continue;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    // --- EXECUTE COMMAND ---
    // delegate the actual running to the helper function.
    // This function doesn't know about redirection, it just writes to stdout/stderr.
    int should_exit = execute_command(args, arg_count);

    // --- RESTORE STDOUT ---
    // Restore stdout to the terminal immediately after execution
    if (stdout_backup != -1) {
       fflush(stdout); // Flush before swapping back
       dup2(stdout_backup, STDOUT_FILENO);
       close(stdout_backup);
    }

    // --- RESTORE STDERR ---
    if (stderr_backup != -1) {
       fflush(stderr); // Good practice
       dup2(stderr_backup, STDERR_FILENO);
       close(stderr_backup);
    }
    
    // Check if it has to exit the shell loop
    if (should_exit) {
        // Free args before breaking
        for (int i = 0; i < arg_count; i++) {
           free(args[i]);
        }
        break;
    }
      
    for (int i = 0; i < arg_count; i++) {
       free(args[i]);
    }

  }
  return 0;
}