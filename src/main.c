#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  const char *builtins[] = {"echo", "exit", "type"};
  size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

  while (1)
  {
    printf("$ ");
    
    char input[100];
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0'; // Remove newline character
    
    if (strncmp(input, "exit", 4) == 0) {
      if (input[4] == '\n' || input[4] == '\0' || input[4] == ' ') {
        break;
      }
    }

    if (strncmp(input, "echo", 4) == 0) {
      
      if (input[4] == '\0') {
        printf("\r\n");
        continue;
      }

      if (input[4] == ' ')
        printf("%s\r\n", input + 5);
      
      continue;
    }

    if (strncmp(input, "type", 4) == 0) {
      
      if (input[4] == '\0') {
        printf("type: usage: type COMMAND\r\n");
        continue;
      }

      if (input[4] == ' ') {
        char *command = input + 5;
        int found = 0;
        for (int i = 0; i < num_builtins; i++) {
          if (strcmp(command, builtins[i]) == 0) {
            printf("%s is a shell builtin\r\n", command);
            found = 1;
            break;
          }
        }
        if (!found) {
          printf("%s: not found\r\n", command);
        }
      }
      continue;
    }

    printf("%s: command not found\r\n", input);
  }
  return 0;
}
