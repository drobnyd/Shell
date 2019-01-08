#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include <string.h>
#include "command_execution.h"
#include "data_structures.h"

/* Maximum size of input */
#define ARG_MAX 10

/* Parser's api */
extern void yy_scan_string(char *);
extern void yyparse(void);
extern void yylex_destroy(void);
extern void restore_exit_value(void);

/* Where parsed commands from bison are stored after calling yyparse() */
extern struct commands_handle *parsed_commands;
/* Buffer for capture state in case of need for restoring it */
extern sigjmp_buf sigint_buf;
/* Parser's return value if greater than 0 parsing error has occured */
extern size_t yyexit_value;

/* Shared with yyerror() in parser */
size_t current_line_num = 1;

/* Forward declarations */

size_t input_too_large(const char *input);


int
main(int argc, char **argv) {
    // Try running with -c argument
    c_option_run(argc, argv);
    // If -c was not applicable try other possibilites
    if (argc == 1) {
        set_sigint_handler();
        interactive_mode_loop();
    } else if (argc == 2) {
        noninteractive_run(argv[1]);
    }
    // TODO print usage
    exit(EXIT_FAILURE);
}

/* Tokenize and parse commands to parsed_commands variable */
void
get_commands(char *input) {
    restore_exit_value();
    yy_scan_string(input);
    yyparse();
    yylex_destroy();
}

/* Try to run commands in -c mode. */
/* If possible exit the program after execution */
void
c_option_run(int argc, char *const *argv) {
    int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
        case 'c':
            if (!input_too_large(optarg)){
                get_commands(optarg);
                // If error in parsing exit
                if (yyexit_value)
                    exit(yyexit_value);
                execute_commands(parsed_commands);
                internal_exit();
            }
        }
    }
}

/* Read commands from file */
void
noninteractive_run(const char *filename) {
    FILE *stream = fopen(filename, "r");
    if (!stream) {
        exit(EXIT_FAILURE);
    }
    char *input = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&input, &len, stream)) != -1) {
        if (!input_too_large(input)){
            get_commands(input);
            // If error in parsing exit
            if (yyexit_value)
                exit(yyexit_value);
            execute_commands(parsed_commands);
        }
        current_line_num++;
    }
    free(input);
    fclose(stream);
    internal_exit();
}

void
interactive_mode_loop() {
    char *input, shell_prompt[100];
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);
    // Interactive mode
    while (1) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof (shell_prompt), "mysh:%s$ ",
            getcwd(NULL, 1024));
        // Set return point for case of ^C
        while (sigsetjmp(sigint_buf, 1) != 0) {
        }
        // Display prompt and read input
        input = readline(shell_prompt);
        // Check for EOF(^D)
        if (!input) // If is EOF exit with the last command's value
            internal_exit();
        if (!input_too_large(input)){
            get_commands(input);
            // If error in parsing don't execute the line
            if (yyexit_value)
                set_exit_value(yyexit_value);
            else // Execute read commands and save into parsed_commands
                execute_commands(parsed_commands);
            // Add input to history.
            add_history(input);
        }
        // Free input.
        free(input);
    }
}

size_t 
input_too_large(const char *input){
    if (strlen(input) > ARG_MAX) {
        fprintf(stderr, "Input cannot be larger than %d bytes\n", ARG_MAX);
        return (1);
    }
    return (0);
}
