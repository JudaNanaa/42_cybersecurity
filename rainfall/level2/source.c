#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void p(void) {
	unsigned int	check;
	char			str[76];

    fflush(stdout);
    gets(str);

	check = (unsigned int)__builtin_return_address(0);
    if ((check & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n", check);
        _exit(1);
    }
    puts(str);
    strdup(str);
    return;
}

int main(void) {
	p();
	return 0;
}
