#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include "command_execution.h"

extern void yyparse(void);
extern void yy_scan_string(char*);
extern void yylex_destroy(void);
extern struct commands_handle *parsed_commands; // Where parsed commands from bison are stored after calling yyparse()
extern sigjmp_buf sigint_buf;


int main(int argc, char **argv)
{
	// If is called via the -c argument
	int c;
	while ((c = getopt (argc, argv, "c:")) != -1) {
		switch (c)
		{
			case 'c':
				get_commands(optarg);
				execute_commands(parsed_commands);
				return 0; // TODO exit value
			case '?':
				if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else
				fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
				return 1;
			default:
				abort ();
		}
	}
	if (argc == 2){ // TODO error handling
		printf("TODO");
		return 0;
	}
	set_sigint_handler();
	interactive_mode_loop();
    return 0;
}

void get_commands(char *input){
	yy_scan_string(input);
	yyparse();
	yylex_destroy();
}

void interactive_mode_loop(){
	char* input, shell_prompt[100];
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);
	// Interactive mode
    for(;;) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "mysh:%s$ ", getcwd(NULL, 1024)); // TODO
		while ( sigsetjmp( sigint_buf, 1 ) != 0 );
        // Display prompt and read input
        input = readline(shell_prompt);
        // Check for EOF.
        if (!input)
            break;
		get_commands(input);
		// Execute read commands and save into parsed_commands
		execute_commands(parsed_commands);
        // Add input to history.
        add_history(input);
        // Free input.
        free(input);
    }
}
