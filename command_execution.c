#include "command_execution.h"
#include "internal_commands.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/queue.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

/* -1 if the process has no children. */
/* Otherwise contains a positive number with child's PID */
pid_t pid_fork = -1;
size_t exit_value = 0;
sigjmp_buf sigint_buf;

void
execute_commands(struct commands_handle *to_execute) {
    if (!to_execute)
        return; // Nothing to execute, should happen
    // TODO
    char **argv;
    struct command *cc;
    struct argument *ca;
    STAILQ_FOREACH(cc, &to_execute->head, entries) {
        size_t i = 0;
        char **tmp;
        argv = malloc(sizeof (char *)); // TODO fail
        argv[i++] = cc->command_name;
        STAILQ_FOREACH(ca, &cc->arguments_handle->head, entries) {
            tmp = realloc(argv, (i+1) * sizeof (char *));
            if (!tmp) {
                // TODO fail
            } else
                argv = tmp;
            argv[i++] = ca->argument_value;
        }
        tmp = realloc(argv, (i+1) * sizeof (char *));
        if (!tmp) {
            // TODO fail
        } else
            argv = tmp;
        argv[i] = NULL;
        if (!exec_internal_command(argv)) {
            exec_child_process(argv);
        }
    }
    deallocate_commands(to_execute);
    free(argv);
}

void
exec_child_process(char *const argv[]) {
    pid_fork = fork(); // Create child process
    if (pid_fork < 0) {
        fprintf(stderr, "fork() failed!\n");
    } else if (pid_fork == 0) {
        execvp(argv[0], argv); // Child
        fprintf(stderr, "mysh: %s: No such file or directory\n", argv[0]);
        exit(127);
    } else {
        wait_for_children();
    }
}

// Blocks until the children is running
void
wait_for_children() {
    int status = 0;
    if ((waitpid(pid_fork, &status, 0)) == -1) {
        // TODO print error
    }
    // Collect child's exit value and set it as the shell's exit value
    if (WIFEXITED(status)) {
        exit_value = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        exit_value = WTERMSIG(status) + 128;
    }
    pid_fork = -1; // Restore no child mode
}

int8_t
exec_internal_command(char *const argv[]) {
    if (strcmp(argv[0], "exit") == 0) {
        internal_exit();
    } else if (strcmp(argv[0], "cd") == 0) {
        internal_cd(argv[1]);
        return (1);
    }
    return (0);
}

void
set_exit_value(size_t val) {
    exit_value = val;
}


void
handle_sigint(int signal) {
    if (pid_fork > 1) { // A child exists, propagate the signal
        fprintf(stderr, "Killed by signal %d.\n", SIGINT);
        kill(pid_fork, SIGINT);
    } else if (pid_fork == -1) {
        // Offer a new line
        fprintf(stdout, "\n");
        // Jump to setlongjmp state saved in sigint_buf
        siglongjmp(sigint_buf, 1);
    }
    // TODO Assert non-valid invariant
}

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
