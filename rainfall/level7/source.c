#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char c[50]; /* size ??? */

void m(void) {
  time_t t;

  t = time(0);
  printf("%s - %d\n", c, (int)t);
  return;
}

int main(int ac, char **av) {
  int *str_1;
  int *str_2;
  FILE *pass;

  str_1 = malloc(8);
  str_1[0] = 1;
  str_1[1] = (int)malloc(8);

  str_2 = malloc(8);
  str_2[0] = 2;
  str_2[1] = (int)malloc(8);

  strcpy((char *)str_1[1], av[1]);
  strcpy((char *)str_2[1], av[2]);

  pass = fopen("/home/user/level8/.pass", "r");
  fgets(c, 68, pass);
  puts("~~");
  return 0;
}
