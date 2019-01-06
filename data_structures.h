#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <sys/queue.h>

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
init_argument(void);

struct command* 
init_command(void);

struct arguments_handle * 
init_argument_list(void);

struct commands_handle * 
init_command_list(void);

void 
argument_list_insert_tail(struct arguments_handle *where, struct argument *what);

void 
command_list_insert_head(struct commands_handle *where, struct command *what);

#endif
