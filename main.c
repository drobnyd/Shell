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
extern size_t yyexit_value;

size_t current_line_num = 1;

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
				internal_exit();
			case '?':
				if (optopt == 'c')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else
					fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
			default:
				exit(EXIT_FAILURE);
		}
	}
	if (argc == 2){ // TODO error handling
		FILE *stream = fopen(argv[1], "r");
		if(!stream){
        	exit(EXIT_FAILURE);
		}
        char *input = NULL;
        size_t len = 0;
        ssize_t nread;
		while ((nread = getline(&input, &len, stream)) != -1) {
            get_commands(input);
			if (yyexit_value){ // If error in parsing has occured, exit with value of the error
				exit(yyexit_value);
			}
			// Execute read commands and save into parsed_commands
			execute_commands(parsed_commands);
			current_line_num++;
        }
        free(input);
        fclose(stream);
		// TODO
		internal_exit();
	}
	set_sigint_handler();
	interactive_mode_loop();
    internal_exit();
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
		while ( sigsetjmp( sigint_buf, 1 ) != 0 )
			;
        // Display prompt and read input
        input = readline(shell_prompt);
        // Check for EOF.
        if (!input) // Case of ^D exit with the last value
            internal_exit();
		get_commands(input);
		// Execute read commands and save into parsed_commands
		execute_commands(parsed_commands);
        // Add input to history.
        add_history(input);
        // Free input.
        free(input);
    }
}
