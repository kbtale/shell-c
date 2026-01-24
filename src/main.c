#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  char input[100];
  fgets(input, sizeof(input), stdin);

  input[strlen(input) - 1] = '\0'; // Remove newline character
  printf("%s: command not found\r\n", input);

  return 0;
}
