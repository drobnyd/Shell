#include "run_modes.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include <string.h>
#include <fcntl.h>
#include "command_execution.h"
#include "data_structures.h"
#include "parser_caller.h"

/* Maximum size of input */
#define ARG_MAX 32000

/* Buffer for capturing a state of the program and then its restoration */
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
                if (get_parser_return_value() != 0)
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
    int fd = open(filename, O_RDONLY);
    if (fd == -1){
        // TODO error
    } 
    char *buffer;
    ssize_t line_size = 0;
    size_t line_num = 1;
    buffer = malloc(sizeof (char));
    while (read(fd, &buffer[line_size], 1) > 0) { // TODO if -1 is the result -> error
        if (buffer[line_size] == '\n' || buffer[line_size] == 0x0) {
            buffer[line_size] = '\0';
            if (!input_too_large(buffer)){
                struct commands_handle *to_execute = parse(buffer, line_num);
                // If error in parsing exit
                if (get_parser_return_value() != 0)
                    exit(get_parser_return_value());
                execute_commands(to_execute);
            }
            // Reset everything and move on the next line
            free(buffer);
            line_num++;
            line_size = 0;
            buffer = malloc(sizeof (char));
        }
        else {
            line_size++;
            buffer = realloc(buffer, line_size * sizeof (char)); // TODO error check
        }
    }
    free(buffer);
    close(fd);
    internal_exit();
}

void
interactive_mode_loop() {
    // Register sigint handler
    set_sigint_handler();
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);
    char *input, shell_prompt[100];
    while (1) { // While not exited by ^D
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
            if (get_parser_return_value() != 0)
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
