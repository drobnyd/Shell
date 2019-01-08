#include "parser_caller.h"
/* Parser's api */
extern void yy_scan_string(char *);
extern void yyparse(void);
extern void yylex_destroy(void);
extern void restore_exit_value(void);

/* Where parsed commands from bison are stored after calling yyparse() */
extern struct commands_handle *parsed_commands;
/* Parser's return value if greater than 0 parsing error has occured */
extern size_t yyexit_value;

/* Shared with yyerror() in parser */
size_t current_line_num;

/* Return tokenize and parse commands */
struct commands_handle *
parse(char *input, size_t line) {
    current_line_num = line;
    restore_exit_value();
    yy_scan_string(input);
    yyparse();
    yylex_destroy();
    return parsed_commands;
}

size_t get_parser_return_value(){
    return (yyexit_value);
}
