#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include "command_execution.h"
#include "data_structures.h"

/* Parser's api */
extern void yy_scan_string(char *);
extern void yyparse(void);
extern void yylex_destroy(void);
extern void restore_exit_value(void);

/* Where parsed commands from bison are stored after calling yyparse() */
extern struct commands_handle *parsed_commands;
/* Buffer for capture state in case of need for restoring it */
extern sigjmp_buf sigint_buf;
/* Parser's return value */
extern size_t yyexit_value;

/* Shared with yyerror() in parser (TODO) */
size_t current_line_num = 1;

int
main(int argc, char **argv) {
    // If is called with the -c argument
    int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
        case 'c':
            get_commands(optarg);
            // If error in parsing exit
            if (yyexit_value)
                exit(yyexit_value);
            execute_commands(parsed_commands);
            internal_exit();
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        default:
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 2) {
        FILE *stream = fopen(argv[1], "r");
        if (!stream) {
            exit(EXIT_FAILURE);
        }
        char *input = NULL;
        size_t len = 0;
        ssize_t nread;
        while ((nread = getline(&input, &len, stream)) != -1) {
            get_commands(input);
            // Exit if error in parsing with the correspoding value
            if (yyexit_value)
                exit(yyexit_value);
            execute_commands(parsed_commands);
            current_line_num++;
        }
        free(input);
        fclose(stream);
        internal_exit();
    }
    set_sigint_handler();
    interactive_mode_loop();
    internal_exit();
}

void
get_commands(char *input) {
    restore_exit_value();
    yy_scan_string(input);
    yyparse();
    yylex_destroy();
}

void
interactive_mode_loop() {
    char *input, shell_prompt[100];
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);
    // Interactive mode
    for (;;) {
        // Create prompt string from user name and current working directory.
        // TODO
        snprintf(shell_prompt, sizeof (shell_prompt), "mysh:%s$ ",
            getcwd(NULL, 1024));
        while (sigsetjmp(sigint_buf, 1) != 0) {
        }
        // Display prompt and read input
        input = readline(shell_prompt);
        // Check for EOF.
        if (!input) // Case of ^D exit with the last value
            internal_exit();
        get_commands(input);
        // Exit if error in parsing with the correspoding value
        if (yyexit_value)
            set_exit_value(yyexit_value);
        else // Execute read commands and save into parsed_commands
            execute_commands(parsed_commands);
        // Add input to history.
        add_history(input);
        // Free input.
        free(input);
    }
}
