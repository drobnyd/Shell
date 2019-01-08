#include "internal_commands.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Value to exit with */
extern size_t exit_value;

void
internal_exit() {
    exit(exit_value);
}

void
internal_cd(const char *dir) {
    char *target;
    if (dir == NULL) { // Go to $HOME
        target = strdup(getenv("HOME"));
    } else if (strcmp(dir, "-") == 0) { // Go to the previous directory
        target = strdup(getenv("OLDPWD"));
    } else { // Else the argument is a path
        target = strdup(dir);
    }
    if (chdir(target) == 0) { // If succesfully changed update env variables
        setenv("OLDPWD", getenv("PWD"), 1);
        setenv("PWD", target, 1);
    } else {
        fprintf(stderr, "cd: %s: No such file or directory\n", target);
    }
    free(target);
}
