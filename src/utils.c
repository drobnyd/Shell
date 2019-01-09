#include <stdlib.h>
#include <stdio.h>

/* Decides if memory at address of ptr was successfully allocated or not */
/* On success 1 is returned, otherwise the shell is exited */
size_t
check_allocation(void *ptr) {
    if (!ptr) {
        fprintf(stderr, "Error, dynamic memory cannot be allocated \n");
        exit(EXIT_FAILURE);
    }
    else {
        return (1);
    }
}