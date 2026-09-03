#include <stdio.h>
#include <string.h>

char a_user_name[256];

int verify_user_name() {
	puts("verifying username....\n");

	return memcmp(a_user_name, "dat_wil", 7);
}

int verify_user_pass(char *pass) {
	return memcmp(pass, "admin", 5);
}

int main(void) {
	char buf[64];
	int nb;

	memset(buf, 0, 16);
	nb = 0;
	puts("********* ADMIN LOGIN PROMPT *********");
	printf("Enter Username: ");
	fgets(a_user_name, 256, stdin);
	if (verify_user_name() != 0) {
		puts("nope, incorrect username...\n");
		return 1;
	}
	puts("Enter Password: ");
	fgets(buf, 100, stdin);
	if (verify_user_pass(buf) != 0)
	{
		puts("nope, incorrect password...\n");
		return 1;
	}
	return 0;
}
