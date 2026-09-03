#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *auth;
char *service;


int main(void) {
	char input[32];

	while (1) {
		printf("%p, %p \n", auth, service);

		if (fgets(input, 128, stdin) == 0) {
			break;
		}
		if (strncmp(input, "auth ", 5) == 0) {
			auth = malloc(4);
			if (strlen(input + 5) <= 30) {
				strcpy(auth, input + 5);
			}
		}
		else if (strncmp(input, "reset", 5) == 0) {
			free(auth);
		}
		else if (strncmp(input, "service", 6) == 0) {
			service = strdup(input + 7);
		}
		else if (strncmp(input, "login", 5) == 0) {
			if (*(int *)(auth + 32) != 0) {
				system("/bin/sh");
			} else {
				fwrite("Password:\n", 1, 10, stdout);
			}
		}
	}
	return 0;
}
