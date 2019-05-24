#include "parser_caller.h"

/* Parser's api */
extern void yy_scan_string(char *);
extern void yyparse(void);
extern void yylex_destroy(void);
extern void restore_parsers_exit_code(void);

/* Where parsed commands from bison are stored after calling yyparse() */
extern pipe_list *parsed_commands;

/* Parser's return value if greater than 0 parsing error has occured */
extern size_t yyexit_code;

/* Shared with yyerror() in parser */
size_t current_line_num;

/* Return tokenize and parse commands */
pipe_list *
parse(char *input, size_t line) {
	current_line_num = line;
	restore_parsers_exit_code();
	yy_scan_string(input);
	yyparse(); // Sets yyexit_code
	yylex_destroy();
	return (parsed_commands);
}

/* If non-zero is returned an error has occurred when parsing */
size_t
get_parser_exit_code() {
	return (yyexit_code);
}
