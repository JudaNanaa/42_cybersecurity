#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	int i;			  //  [esp+0x8c]
	char buffer[100]; //  [esp+0x28]

	i = 0;
	fgets(buffer, 100, stdin);
	i = 0;


	while (1) {
		if (strlen(buffer) <= i) {
			printf(buffer);
			exit(0);
			
		}
		if (buffer[i] > 64 && buffer[i] <= 90)
				buffer[i] ^= 32; // met en minuscule
		i++;
	}

}
