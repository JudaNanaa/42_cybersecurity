#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

int auth(char *buff, unsigned int nb) {
	size_t len;
	int ret_val;
	long ptrace_ret;
	unsigned int serial;

	len = strcspn(buff, "\n");
	buff[len] = '\0';
	len = strnlen(buff, 0x20);
	if ((int)len < 6) {
		ret_val = 1;
	} else {
		ptrace_ret = ptrace(PTRACE_TRACEME);
		if (ptrace_ret == -1) {
			puts("\x1b[32m.---------------------------.");
			puts("\x1b[31m| !! TAMPERING DETECTED !!  |");
			puts("\x1b[32m\'---------------------------\'");
			ret_val = 1;
		} else {
			serial = ((int)buff[3] ^ 0x1337U) + 0x5eeded;
			for (int i = 0; i < (int)len; i = i + 1) {
				if (buff[i] < ' ') {
					return 1;
				}
				serial = serial + ((int)buff[i] ^ serial) % 0x539;
			}
			if (nb == serial) {
				ret_val = 0;
			} else {
				ret_val = 1;
			}
		}
	}
	return ret_val;
}

int main() {
	char buff[32];	 // [esp + 0x2c]
	unsigned int nb; // [esp + 0x28]

	puts("***********************************");
	puts("*\t\tlevel06\t\t  *");
	printf("-> Enter Login: ");
	fgets(buff, 32, stdin);
	puts("***********************************");
	puts("***** NEW ACCOUNT DETECTED ********");
	puts("***********************************");
	printf("-> Enter Serial: ");
	__isoc99_scanf("%u", nb);
	if (auth(buff, nb) == 0) {
		puts("Authenticated!");
		system("/bin/sh");
		return 0;
	} else {
		return 1;
	}
}
