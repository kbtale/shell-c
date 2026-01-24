#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");
  
    char input[100];
    fgets(input, sizeof(input), stdin);
    
    if (strncmp(input, "exit", 4) == 0) {
      if (input[4] == '\n' || input[4] == '\0' || input[4] == ' ') {
        break;
      }
    }

    if (strncmp(input, "echo ", 5) == 0) {
      printf("%s\r\n", input + 5);
      continue;
    }
    
    input[strlen(input) - 1] = '\0'; // Remove newline character
    printf("%s: command not found\r\n", input);
  }
  return 0;
}
