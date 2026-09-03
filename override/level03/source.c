#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void clear_stdin() {
	int nb;

	nb = 0;
	while (nb != 10) {
		nb = getchar();
		if (nb == 255)
			break;
	}
}

int get_unum() {
	int nb;

	nb = 0;
	fflush(stdout);
	__isoc99_scanf("%u", &nb);
	clear_stdin();
	return nb;
}

void prog_timeout() { exit(1); }

int decrypt(int nb) {
	char tab2[4];
	int s;
	int len;
	char tab[17];
	int nb;

	nb = 119;
	*(int *)tab[0] = 1971092817;
	*(int *)tab[4] = 1734767456;
	*(int *)tab[8] = 2070311806;
	*(int *)tab[12] = 862026877;
	tab[16] = 0;

	*(int *)tab2[0] = 4294967295;

	len = strlen(tab);
	s = 0;
	if (s == len) {

	}
}

int test(int a1, int a2) {
	int result;
	char v3;

	switch (a2 - a1) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
		result = decrypt(a2 - a1);
		break;
	default:
		v3 = rand();
		result = decrypt(v3);
		break;
	}
	return result;
}

int main(void) {
	int nb;

	srand(time(NULL));
	puts("***********************************");
	puts("*\t\tlevel03\t\t**");
	puts("***********************************");
	printf("Password:");
	__isoc99_scanf("%d", &nb);
	test(nb, 322424845);
}
