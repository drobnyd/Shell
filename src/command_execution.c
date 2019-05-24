#include "command_execution.h"
#include "internal_commands.h"
#include "utils.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/queue.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <err.h>
#include <fcntl.h>

/* -1 if the process has no children.
 Otherwise contains a positive number with child's PID */
pid_t pid_fork = -1;
size_t exit_code = 0;
sigjmp_buf sigint_buf;

/** Execute one by one commands stored in to_execute */
void
execute_input(pipe_list *to_execute) {

	if (!to_execute)
		return; // Nothing to execute, shouldn't happen

	command_list *cp;
	STAILQ_FOREACH(cp, &to_execute->head, entries) {
		execute_commands_in_pipe(cp);
	}

	pipe_list_deallocate(to_execute);
}

/** Loads command structure to argv executable by execvp */
char **
load_command_to_argv(struct command *cc) {
	char **argv, **tmp;
	size_t i = 0;

	argv = malloc(sizeof (char *));
	check_allocation(argv);
	argv[i++] = cc->command_name;

	struct argument *ca;
	STAILQ_FOREACH(ca, &cc->arguments_handle->head, entries) {
		tmp = realloc(argv, (i+1) * sizeof (char *));
		check_allocation(tmp);
		argv = tmp;
		argv[i++] = ca->argument_value;
	}

	tmp = realloc(argv, (i+1) * sizeof (char *));
	check_allocation(tmp);
	argv = tmp;
	argv[i] = NULL;

	return argv;
}

void
redirect(redirection *redirection, int *in, int *out, int *fd) {
	// Redirection
	if (redirection != NULL) {
		if (redirection->in_file != NULL) {
			//close(in);
			*in = open(redirection->in_file, O_RDONLY);
		}

		if (redirection->out_file_a != NULL) {
			close(fd[1]); // Nothing will be written to the pipe
			*out = open(redirection->out_file_a, O_WRONLY | O_CREAT | O_APPEND, 0666);
		}

		if (redirection->out_file_r != NULL) {
			close(fd[1]); // Nothing will be written to the pipe
			*out = open(redirection->out_file_r, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		}
	}
}

void
execute_commands_in_pipe(command_list *to_execute) {

	if (!to_execute)
		return; // Nothing to execute, shouldn't happen

	struct command *cc;
	/* File descriptors between piped commands */
	int out, in = STDIN_FILENO;

	STAILQ_FOREACH(cc, &to_execute->head, entries) {
		char **argv = load_command_to_argv(cc);
		int fd[2];
		int next_in = STDIN_FILENO; // Input for next command

		if (STAILQ_NEXT(cc,entries) != NULL) {
			if (pipe(fd))
				err(2,"");

			next_in = fd[0];
			out = fd[1];
		} else // Is the last one, write out
			out = STDOUT_FILENO;

		redirect(cc->redirection, &in, &out, fd);

		if (!exec_internal_command(argv))
			exec_child_process(argv, in, out);

		in = next_in;

		free(argv);
	}

}

/** Fork a child process and execute it, parent waits for its end */
void
exec_child_process(char *const argv[], int in, int out) {
	pid_fork = fork(); // Create child process
	if (pid_fork < 0) {
		warn("");
	} else if (pid_fork == 0) { // In child's execution

		if (in != 0) {
			if (dup2(in, 0) != 0)
				warn("");

			if (close(in))
				warn("");
		}

		if (out != 1) {
			if (dup2(out, 1) != 1)
				warn("");

			if (close(out))
				warn("");
		}

		execvp(argv[0], argv);

		err(127,"%s", argv[0]);
	} else { // Parent
		// Parent does not communicate through pipe, close descriptors

		if (in != 0)
			if (close(in))
				warn("");

		if (out != 1)
			if (close(out))
				warn("");

		wait_for_children();
	}
}

/** Blocks until the children is running. Then collects child's return value. */
void
wait_for_children() {
	int status = 0;
	if ((waitpid(pid_fork, &status, 0)) == -1) {
		warn("PID:%d ", pid_fork);
	}
	// Collect child's exit value and set it as the shell's exit value
	if (WIFEXITED(status)) {
		exit_code = WEXITSTATUS(status);
	} else if (WIFSIGNALED(status)) {
		exit_code = WTERMSIG(status) + 128;
	}
	pid_fork = -1; // Restore no child mode
}

/** Returns 0 if no internal command was executed, otherwise 1 */
size_t
exec_internal_command(char *const argv[]) {
	if (strcmp(argv[0], "exit") == 0) {
		internal_exit();
	} else if (strcmp(argv[0], "cd") == 0) {
		internal_cd(argv[1]);
		return (1);
	}
	return (0);
}

/** Sets exit value on the event of exit of the shell */
void
set_exit_code(size_t val) {
	exit_code = val;
}

/** Handling of SIGINT signal of the shell */
void
handle_sigint(int signal) {
	if (pid_fork > 1) { // A child exists, propagate the signal
		warnx("Killed by signal %d.", SIGINT);
		kill(pid_fork, SIGINT);

	} else if (pid_fork == -1) {
		// Offer a new line
		fprintf(stdout, "\n");
		// Jump to setlongjmp state saved in sigint_buf
		siglongjmp(sigint_buf, 1);
	}
	// Else is a non-valid invariant that should never happen
}

/** Registers lookup(handle_sigint) method as SIGINT handler */
void
set_sigint_handler() {
	struct sigaction sa;
	sigset_t set;
	sigemptyset(&set);
	sa.sa_handler = handle_sigint;
	sa.sa_mask = set;
	sa.sa_flags = SA_RESTART;
	// Register the completed struct to SIGINT signal
	// Last arg means the old handler in case of change is not interesting
	(void) sigaction(SIGINT, &sa, NULL);
}
