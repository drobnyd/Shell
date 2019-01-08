#include "data_structures.h"
#include <stdlib.h>

struct argument *
init_argument(void) {
    struct argument *argument =
        (malloc(sizeof (struct argument)));
    argument->argument_value = NULL;
    return (argument);
}

struct command *
init_command(void) {
    struct command *command =
        (malloc(sizeof (struct command)));
    command->command_name = NULL;
    command->arguments_handle = NULL;
    return (command);
}
struct arguments_handle *
init_argument_list(void) {
    struct arguments_handle *arguments =
        malloc(sizeof (struct arguments_handle));
    STAILQ_INIT(&arguments->head);
    return (arguments);
}

struct commands_handle *
init_command_list(void) {
    struct commands_handle *commands =
        malloc(sizeof (struct commands_handle));
    STAILQ_INIT(&commands->head);
    return (commands);
}

void
argument_list_insert_tail(struct arguments_handle *where,
    struct argument *what) {
    STAILQ_INSERT_TAIL(&where->head, what, entries);
}

void
command_list_insert_head(struct commands_handle *where, struct command *what) {
    STAILQ_INSERT_HEAD(&where->head, what, entries);
}

void 
deallocate_arguments(struct arguments_handle *what){
    struct argument *n1,*n2;
    n1 = STAILQ_FIRST(&what->head);
    while (n1 != NULL) {
        n2 = STAILQ_NEXT(n1, entries);
        free(n1->argument_value);
        free(n1);
        n1 = n2;
    }
    free(what);
}

void
deallocate_commands(struct commands_handle *what){
    struct command *n1,*n2;
    n1 = STAILQ_FIRST(&what->head);
    while (n1 != NULL) {
        deallocate_arguments(n1->arguments_handle);
        n2 = STAILQ_NEXT(n1, entries);
        free(n1->command_name);
        free(n1);
        n1 = n2;
    }
    free(what);
}
