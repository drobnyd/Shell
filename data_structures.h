#include <sys/queue.h>
#include <stdlib.h>

struct argument {
    char *argument_value;
    STAILQ_ENTRY(argument) entries;
};

struct command {
    char *command_name;
    struct arguments_handle *arguments_handle;
    STAILQ_ENTRY(command) entries;
};

struct arguments_handle {
    STAILQ_HEAD(argument_list, argument) head;
};

struct commands_handle {
    STAILQ_HEAD(command_list, command) head;
};

struct argument* 
init_argument(void) {
    struct argument *argument = (struct argument *)(malloc(sizeof(struct argument)));
    argument->argument_value = NULL;
    return argument;
}

struct command* 
init_command(void) {
    struct command *command = (struct command *)(malloc(sizeof(struct command)));
    command->command_name = NULL;
    command->arguments_handle = NULL;
    return command;
}
struct arguments_handle * 
init_argument_list(void){
    struct arguments_handle *arguments = (struct arguments_handle *)malloc(sizeof(struct arguments_handle *));
    STAILQ_INIT(&arguments->head);
    return arguments;
}

struct commands_handle * 
init_command_list(void){
    struct commands_handle *commands = (struct commands_handle *)malloc(sizeof(struct commands_handle *));
    STAILQ_INIT(&commands->head);
    return commands;
}

void 
argument_list_insert_tail(struct arguments_handle *where, struct argument *what){
    STAILQ_INSERT_TAIL(&where->head, what, entries); 
}

void 
command_list_insert_head(struct commands_handle *where, struct command *what){
    STAILQ_INSERT_HEAD(&where->head, what, entries); 
}

