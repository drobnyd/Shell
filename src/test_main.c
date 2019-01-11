#include <stdlib.h>
#include <stdio.h>
#include "run_modes.h"

int
main(int argc, char **argv) {
    // Fake input
    freopen(argv[1], "r", stdin);
	interactive_mode_loop();
	exit(EXIT_FAILURE);
}
