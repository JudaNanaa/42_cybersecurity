#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
	int i;			  //  [esp+0x8c]
	char buffer[100]; //  [esp+0x28]

	i = 0;
	fgets(buffer, 100, stdin);
	i = 0;

	while (i < strlen(buffer)) {
		if (buffer[i] > 0x40 && buffer[i] <= 0x5a) {
			buffer[i] ^= 0x20;
		}
		i++;
	}
	printf(buffer);
	exit(0);
}
