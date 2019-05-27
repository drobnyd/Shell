#ifndef PARSER_CALL_H
#define	PARSER_CALL_H
#include <stdlib.h>
#include "data_structures.h"

/* Methods for interaction with parser */

pipe_list *
parse(char *input, size_t line);

size_t
get_parser_exit_code();

#endif
