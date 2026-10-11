#include <linux/prctl.h> /* Definition of PR_* constants */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <wait.h>

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

void enable_timeout_cons() { signal(14, prog_timeout); }

int main() {
	int status;		// [esp + 0x1c]
	char tab[32]; 	// [esp + 0x20]
	long orig_eax;	// [esp + 0xa8]
	pid_t pid;		// [esp + 0xac]

	pid = fork();
	memset(tab, 0, 32);
	orig_eax = 0;
	status = 0;
	if (pid == 0) {
		prctl(PR_SET_PDEATHSIG, SIGHUP);
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		puts("Give me some shellcode, k");
		gets(tab);
	} else {
		while (1) {
			wait(&status);
			if ((WIFEXITED(status)) || (WIFSIGNALED(status))){
				puts("child is exiting...");
				return 0;
			}
			orig_eax = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
			if (orig_eax == 11)
				break;
		}
		puts("no exec() for you");
		kill(pid, SIGKILL);
	}
	return 0;
}
