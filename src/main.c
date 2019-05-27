#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "run_modes.h"

int
main(int argc, char **argv) {
	// Try running with -c argument. If applicable, exit after
	if (c_option_run(argc, argv) == 0) {

		// If -c was not applicable try other possibilities
		if (argc == 1)
			interactive_mode_loop();
		else if (argc == 2)
			noninteractive_run(argv[1]);

	}

	errx(2, "Usages:\nmysh\nmysh -c[command]*\nmysh file.sh\n");
}
