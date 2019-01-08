#ifndef CMD_EXEC_H
#define CMD_EXEC_H

#include <stdlib.h>
#include "data_structures.h"

#define MAX_ARGS 32000

void
execute_commands(struct commands_handle *to_execute);

int8_t
exec_internal_command(char *const argv[]);

void
internal_exit();

void
internal_cd();

void
interactive_mode_loop(void);

void
set_sigint_handler(void);

void
exec_child_process(char *const argv[]);

void
wait_for_children(void);

void
set_exit_value(size_t val);

#endif
