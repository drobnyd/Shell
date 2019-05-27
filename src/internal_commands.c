#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "internal_commands.h"

/* Value to exit with */
extern size_t exit_code;

void
internal_exit() {
	exit(exit_code);
}

void
internal_cd(const char **argv) {
	// First element is "cd"
	const char *dir = argv[1]; // Second is the target dir

	// In the case that the needed variables haven't been set
	// But do not overwrite existing - the last arg is 0
	setenv("PWD", getcwd(NULL, 0), 0);
	setenv("OLDPWD", getenv("PWD"), 0);
	// In such case make the current directory the home one
	setenv("HOME", getenv("PWD"), 0);

	char *target;

	if (dir == NULL) // Go to $HOME
		target = strdup(getenv("HOME"));
	else if (strcmp(dir, "-") == 0) // Go to the previous directory
		target = strdup(getenv("OLDPWD"));
	else { // Else the argument is a path

		if (argv[2] != NULL) { // It can be only a single path
			warnx("cd: too many arguments");
			exit_code = 1;
			return;
		}

		target = strdup(dir);
	}

	if (chdir(target) == 0) {
		// If successfully changed, update env variables
		setenv("OLDPWD", getenv("PWD"), 1);
		setenv("PWD", target, 1);

	} else {
		warn("%s", target);
		exit_code = 1;
	}


	free(target);
}
