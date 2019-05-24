#ifndef DATA_STRUCTS_H
#define	DATA_STRUCTS_H

#include <sys/queue.h>

// Arguments

/** Arguments of a command */
struct argument {
	char *argument_value;

	STAILQ_ENTRY(argument) entries;
};

struct argument *
init_argument(void);

typedef struct argument_list {
	STAILQ_HEAD(arguments, argument) head;
} argument_list;

void
argument_list_deallocate(argument_list *what);

argument_list *
argument_list_init(void);

void
argument_list_insert_tail(argument_list *where,
struct argument *what);

// Redirection
typedef struct redirection {
	char *in_file;
	// Mutually exclusive
	/* File to append output to */
	char *out_file_a;
	/* File to rewrite with output */
	char *out_file_r;
} redirection;

redirection *
redirection_init(void);

void
redirection_deallocate(redirection *what);

// Commands

/** Command and its arguments */
struct command {
	char *command_name;
	argument_list *arguments_handle;
	redirection *redirection;
	STAILQ_ENTRY(command) entries;
};

struct command *
command_init(void);

void
command_deallocate(struct command *what);

typedef struct command_list {
	STAILQ_HEAD(commands, command) head;

	STAILQ_ENTRY(command_list) entries;
} command_list;

command_list *
command_list_init(void);

void
command_list_insert_head(command_list *where, struct command *what);

void
command_list_deallocate(command_list *what);

// Pipes

/** Colon of commands delimited by a pipe */
typedef struct pipe_list {
	STAILQ_HEAD(pipes, command_list) head;
} pipe_list;

pipe_list *
pipe_list_init(void);

void
pipe_list_insert_head(pipe_list *where,
command_list *what);

void
pipe_list_insert_simple_head(pipe_list *where,
struct command *what);

void
pipe_list_deallocate(pipe_list *what);

#endif
