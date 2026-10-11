#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void log_wrapper(FILE *a1, const char *a2, const char *a3) {
	char dest[264];

	strcpy(dest, a2);
	snprintf(&dest[strlen(dest)], 254 - strlen(dest), a3);
	dest[strcspn(dest, "\n")] = 0;
	fprintf(a1, "LOG: %s\n", dest);
}

int main(int argc, const char **argv, const char **envp) {
	FILE *logfile;
	FILE *flag;
	int fd;
	char buf;
	char dest[104];

	buf = -1;
	if (argc != 2)
		printf("Usage: %s filename\n", *argv);
	logfile = fopen("./backups/.log", "w");
	if (logfile == NULL) {
		printf("ERROR: Failed to open %s\n", "./backups/.log");
		exit(1);
	}
	log_wrapper(logfile, "Starting back up: ", argv[1]);
	flag = fopen(argv[1], "r");
	if (flag == NULL) {
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}
	strcpy(dest, "./backups/");
	strncat(dest, argv[1], 99 - strlen(dest));
	fd = open(dest, 193, 432);
	if (fd < 0) {
		printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
		exit(1);
	}
	while (1) {
		buf = fgetc(flag);
		if (buf == -1)
			break;
		write(fd, &buf, 1);
	}
	log_wrapper(logfile, "Finished back up ", argv[1]);
	fclose(flag);
	close(fd);
	return 0;
}
