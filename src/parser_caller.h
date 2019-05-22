#ifndef PARSER_CALL_H
#define	PARSER_CALL_H
#include "data_structures.h"
#include <stdlib.h>

/* Methods for interaction with parser */

struct pipe_handle *
parse(char *input, size_t line);

size_t
get_parser_exit_code();

#endif
