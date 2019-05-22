#include "data_structures.h"
#include "utils.h"
#include <stdlib.h>


struct argument *
init_argument(void) {
	struct argument *argument =
		malloc(sizeof (struct argument));
	check_allocation(argument);

	argument->argument_value = NULL;
	return (argument);
}

struct arguments_handle *
init_argument_list(void) {
	struct arguments_handle *arguments =
			malloc(sizeof (struct arguments_handle));
	check_allocation(arguments);

	STAILQ_INIT(&arguments->head);
	return (arguments);
}

struct redirection *
init_redirection(void) {
	struct redirection *redirection =
			malloc(sizeof (struct redirection));
	check_allocation(redirection);

	redirection->in_file = NULL;
	redirection->out_file_r = NULL;
	redirection->out_file_a = NULL;
	return (redirection);
}

struct command *
init_command(void) {
	struct command *command =
		malloc(sizeof (struct command));
	check_allocation(command);

	command->command_name = NULL;
	command->arguments_handle = NULL;
	command->redirection = NULL;
	return (command);
}

struct commands_handle *
init_command_list(void) {
	struct commands_handle *commands =
			malloc(sizeof (struct commands_handle));
	check_allocation(commands);

	STAILQ_INIT(&commands->head);
	return (commands);
}

struct pipe_handle *
init_pipe_list(void) {
	struct pipe_handle *commands =
			malloc(sizeof (struct pipe_handle));
	check_allocation(commands);

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
pipe_list_insert_head(struct pipe_handle *where,
	struct commands_handle *what){
		STAILQ_INSERT_HEAD(&where->head, what, entries);
}

void
pipe_list_insert_simple_head(struct pipe_handle *where,
	struct command *what){
		struct commands_handle *to_add = init_command_list();
		command_list_insert_head(to_add, what);
		pipe_list_insert_head(where, to_add);
}

void
deallocate_arguments(struct arguments_handle *what) {
	struct argument *n1, *n2;
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
deallocate_commands(struct commands_handle *what) {
	struct command *n1, *n2;
	n1 = STAILQ_FIRST(&what->head);
	while (n1 != NULL) {
		n2 = STAILQ_NEXT(n1, entries);
		deallocate_command(n1);
		n1 = n2;
	}
	free(what);
}

void
deallocate_command(struct command *what){
	deallocate_arguments(what->arguments_handle);
	deallocate_redirection(what->redirection);
	free(what->command_name);
	free(what);
}

void
deallocate_redirection(struct redirection *what){
	if (what == NULL)
		return;

	free(what->in_file);
	free(what->out_file_a);
	free(what->out_file_r);
	free(what);
}

void
deallocate_pipe(struct pipe_handle *what){
	// TODO
}
