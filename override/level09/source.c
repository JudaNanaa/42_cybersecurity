#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int secret_backdoor(void) {
	char s[128];

	fgets(s, 128, stdin);
	return system(s);
}

int set_username(char *a1) {
	char s[140];
	int i;

	memset(s, 0, 128);
	puts(">: Enter your username");
	printf(">>: ");
	fgets(s, 128, stdin);
	for (i = 0; i <= 40 && s[i] != '\0'; ++i)
		a1[i + 140] = s[i];
	return printf(">: Welcome, %s", a1 + 140);
}

char *set_msg(char *a1) {
	char s[1024];

	memset(s, 0, sizeof(s));
	puts(">: Msg @Unix-Dude");
	printf(">>: ");
	fgets(s, 1024, stdin);
	return strncpy(a1, s, *(int *)(a1 + 180));
}

int handle_msg(void) {
	char tab[192];

	memset(tab + 140, 0, 40);
	*(int *)(tab + 180) = 140;
	set_username(tab);
	set_msg(tab);
	return puts(">: Msg sent!");
}

int main(int argc, const char **argv, const char **envp) {
	puts("--------------------------------------------\n"
		 "|   ~Welcome to l33t-m$n ~    v1337        |\n"
		 "--------------------------------------------");
	handle_msg();
	return 0;
}
