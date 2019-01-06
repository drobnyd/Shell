#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "command_execution.h"


extern void yyparse(void);
extern void yy_scan_string(char*);
extern void yylex_destroy(void);
extern struct commands_handle *parsed_commands; // Where parsed commands from bison are stored after calling yyparse()

int main()
{
	// TODO if is called with the -c argument

    char* input, shell_prompt[100];

    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);


    for(;;) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "mysh:%s$ ", getcwd(NULL, 1024));

        // Display prompt and read input (NB: input must be freed after use)...
        input = readline(shell_prompt);

        // Check for EOF.
        if (!input)
            break;

        // Add input to history.
        add_history(input);

        // Do stuff...
		yy_scan_string(input);
		yyparse();
		yylex_destroy();

		// Loads commands into stack
		add_history(input);

        // Free input.
        free(input);

		// Execute read commands
		execute_commands(parsed_commands);

    }
    return 0;
}