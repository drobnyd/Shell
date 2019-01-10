#ifndef	CMD_EXEC_H
#define	CMD_EXEC_H
#include <stdlib.h>
#include "data_structures.h"

void
execute_commands(struct commands_handle *to_execute);

size_t
exec_internal_command(char *const argv[]);

void
set_sigint_handler(void);

void
exec_child_process(char *const argv[]);

void
wait_for_children(void);

void
set_exit_code(size_t val);

#endif
