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

struct arguments_handle {
	STAILQ_HEAD(argument_list, argument) head;
};

void
deallocate_arguments(struct arguments_handle *what);

struct arguments_handle *
init_argument_list(void);

void
argument_list_insert_tail(struct arguments_handle *where,
struct argument *what);

// Redirection
struct redirection {
	char *in_file;
	// Mutually exclusive
	/* File to append output to */
	char *out_file_a;
	/* File to rewrite with output */
	char *out_file_r;
};

struct redirection *
init_redirection(void);

void
deallocate_redirection(struct redirection *what);

// Commands

/** Command and its arguments */
struct command {
	char *command_name;
	struct arguments_handle *arguments_handle;
	struct redirection *redirection;
	STAILQ_ENTRY(command) entries;
};

struct command *
init_command(void);

void
deallocate_command(struct command *what);

struct commands_handle {
	STAILQ_HEAD(command_list, command) head;
	STAILQ_ENTRY(commands_handle) entries;
};

struct commands_handle *
init_command_list(void);

void
command_list_insert_head(struct commands_handle *where, struct command *what);

void
deallocate_commands(struct commands_handle *what);

// Pipes

/** Colon of commands delimited by a pipe */
struct pipe_handle {
	STAILQ_HEAD(composite_list, commands_handle) head;
};

struct pipe_handle *
init_pipe_list(void);

void
pipe_list_insert_head(struct pipe_handle *where,
struct commands_handle *what);

void
pipe_list_insert_simple_head(struct pipe_handle *where,
struct command *what);

void
deallocate_pipe(struct pipe_handle *what);

#endif
