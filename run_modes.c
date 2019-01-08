#include "run_modes.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include <string.h>
#include "command_execution.h"
#include "data_structures.h"
#include "parser_caller.h"
/* Maximum size of input */
#define ARG_MAX 10

/* Buffer for capture state in case of need for restoring it */
extern sigjmp_buf sigint_buf;

/* Try to run commands in -c mode. */
/* If possible exit the program after execution */
void
c_option_run(int argc, char *const *argv) {
    int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
        case 'c':
            if (!input_too_large(optarg)){
                struct commands_handle *to_execute = parse(optarg, 1);
                // If error in parsing exit
                if (get_parser_return_value())
                    exit(get_parser_return_value());
                execute_commands(to_execute);
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
    size_t line_num = 1;
    while ((nread = getline(&input, &len, stream)) != -1) {
        if (!input_too_large(input)){
            struct commands_handle *to_execute = parse(input, line_num);
            // If error in parsing exit
            if (get_parser_return_value())
                exit(get_parser_return_value());
            execute_commands(to_execute);
        }
        line_num++;
    }
    free(input);
    fclose(stream);
    internal_exit();
}

void
interactive_mode_loop() {
    // Register sigint handler
    set_sigint_handler();
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
            struct commands_handle *to_execute = parse(input, 1);
            // If error in parsing don't execute the line
            if (get_parser_return_value())
                set_exit_value(get_parser_return_value());
            else
                execute_commands(to_execute);
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
