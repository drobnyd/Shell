#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Maximum size of input */
#define	ARG_MAX 32000


/* Decides if memory at address of ptr was successfully allocated or not */
/* On success 1 is returned, otherwise the shell is exited */
size_t
check_allocation(void *ptr) {
	if (!ptr) {
		fprintf(stderr, "Error, dynamic memory cannot be allocated \n");
		exit(EXIT_FAILURE);
	} else {
		return (1);
	}
}

size_t
input_too_large(const char *input) {
	if (strlen(input) > ARG_MAX) {
		fprintf(stderr,
			"Input cannot be larger than %d bytes\n", ARG_MAX);
		return (1);
	}
	return (0);
}
