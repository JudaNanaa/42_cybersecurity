#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(void) {
  char password[112];
  char string_pass[48];
  char username[100];
  int nb_read;
  FILE *stream;

  memset(username, 0, sizeof(username));
  memset(string_pass, 0, 41);
  memset(password, 0, 100);
  stream = NULL;
  nb_read = 0;
  stream = fopen("/home/users/level03/.pass", "r");
  if (stream == NULL) {
    fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
    exit(1);
  }
  nb_read = fread(string_pass, 1, 41, stream);
  string_pass[strcspn(string_pass, "\n")] = 0;
  if (nb_read != 41) {
    fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
    fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
    exit(1);
  }
  fclose(stream);
  puts("===== [ Secure Access System v1.0 ] =====");
  puts("/***************************************\\");
  puts("| You must login to access this system. |");
  puts("\\**************************************/");
  printf("--[ Username: ");
  fgets(username, 100, stdin);
  username[strcspn(username, "\n")] = 0;
  printf("--[ Password: ");
  fgets(password, 100, stdin);
  password[strcspn(password, "\n")] = 0;
  puts("*****************************************");
  if (strncmp(string_pass, password, 41) != 0) {
    printf(username);
    puts(" does not have access!");
    exit(1);
  }
  printf("Greetings, %s!\n", username);
  system("/bin/sh");
  return 0;
}
