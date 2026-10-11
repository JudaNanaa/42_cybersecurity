#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int clear_stdin() {
	int result;

	do
		result = getchar();
	while ((char)result != '\n' && (char)result != EOF);
	return result;
}

unsigned int get_unum() {
	unsigned int nb;

	nb = 0;
	fflush(stdout);
	scanf("%u", &nb);
	clear_stdin();
	return nb;
}

void prog_timeout() { exit(1); }

int store_number(int a1) {
	unsigned int number;
	unsigned int index;

	printf(" Number: ");
	number = get_unum();
	printf(" Index: ");
	index = get_unum();
	if (index % 3 == 0 || number >> 0x18 == 0xb7) {
		puts(" *** ERROR! ***");
		puts("   This index is reserved for wil!");
		puts(" *** ERROR! ***");
		return 1;
	} else {
		*(int *)(a1 + 4 * index) = number;
		return 0;
	}
}

int read_number(int a1) {
	unsigned int unum;

	printf(" Index: ");
	unum = get_unum();
	printf(" Number at data[%u] is %u\n", unum, *(int *)(a1 + 4 * unum));
	return 0;
}

int main(int argc, const char **argv, const char **envp) {
	char tab[400];
	int number;
	char buff[20];

	number = 0;
	memset(buff, 0, 20);
	memset(tab, 0, sizeof(tab));
	while (*argv != NULL) {
		memset((void *)*argv, 0, strlen(*argv));
		++argv;
	}
	while (*envp != NULL) {
		memset((void *)*envp, 0, strlen(*envp));
		++envp;
	}
	puts("----------------------------------------------------\n"
		 "  Welcome to wil's crappy number storage service!   \n"
		 "----------------------------------------------------\n"
		 " Commands:                                          \n"
		 "    store - store a number into the data storage    \n"
		 "    read  - read a number from the data storage     \n"
		 "    quit  - exit the program                        \n"
		 "----------------------------------------------------\n"
		 "   wil has reserved some storage :>                 \n"
		 "----------------------------------------------------\n");
	while (1) {
		printf("Input command: ");
		number = 1;
		fgets(buff, 20, stdin);
		buff[strlen(buff) - 1] = 0;
		if (memcmp(buff, "store", 5u) == 0) 
			number = store_number((int)tab);
		else if (memcmp(buff, "read", 4u) == 0) 
			number = read_number((int)tab);
		else if (memcmp(buff, "quit", 4u) == 0)
			return 0;
		if (number != 0)
			printf(" Failed to do %s command\n", buff);
		else
			printf(" Completed %s command successfully\n", buff);
		memset(buff, 0, 20);
	}
}
