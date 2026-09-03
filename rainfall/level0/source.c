#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int ac, char **av) {
    int number;
	char *tab[2];
    uid_t uid;
    gid_t gid;

    number = atoi(av[1]);
	if (number == 423)
	{
        tab[0] = strdup("/bin/sh");
		tab[1] = NULL;
		gid = getegid();
        uid = geteuid();
        setresgid(gid, gid, gid);
        setresuid(uid, uid, uid);
        execv("/bin/sh", tab);
    }
	else
	{
		fwrite("No !\n", 1, 5, stderr);
    }
    return 0;
}
