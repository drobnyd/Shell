%{
	#include <stdio.h>
	#include <string.h>
	#include "data_structures.h"
	void yyerror(const char *msg);
	int yylex();
	pipe_list *parsed_commands = NULL;
	extern size_t current_line_num;
	size_t yyexit_code = 0;
%}

%union {
	char *str_val;
	int int_val;
	struct command_list *command_list;
	struct argument_list *argument_list;
	struct pipe_list *pipe_list;
	struct command *command_ptr;
	struct redirection *redirection_ptr;
}

%error-verbose

/* Declare tokens */ 
%token<str_val> WORD "word"
%token<str_val> QUOTED "quote"
%token SEMICOLON ";"
%token PIPELINE "|"
%token END "EOF"
%token EOL "\\n"
%token IN_REDIRECTION "<"
%token OUT_REDIRECTION ">"

%destructor { free($$); } WORD QUOTED
%destructor { command_deallocate($$); } command
%destructor { argument_list_deallocate($$); } arguments
%destructor { command_list_deallocate($$); } pipe
%destructor { pipe_list_deallocate($$); } command_line
%destructor { redirection_deallocate($$); } redirection

%type<command_list> pipe
%type<argument_list> arguments
%type<command_ptr> command
%type<pipe_list> command_line
%type<redirection_ptr> redirection
%%

input: end /* Empty line is ignored */ { parsed_commands = NULL; YYACCEPT; }
	| command_line end { 
		parsed_commands = $1;
		YYACCEPT; 
	} 
	;

end: EOL 
	| END
	;

/* Commands on a single line */
/* right recursion the current command is added as the head */
command_line: command {
		$$ = pipe_list_init();
		pipe_list_insert_simple_head($$, $1);
	}
	| command SEMICOLON { 
		$$ = pipe_list_init();
		pipe_list_insert_simple_head($$, $1);
	} 
	| command SEMICOLON command_line {
		pipe_list_insert_simple_head($3, $1);
		$$ = $3;
	}
	| pipe {
		$$ = pipe_list_init();
		pipe_list_insert_head($$, $1);
	}
	| pipe SEMICOLON {
         	$$ = pipe_list_init();
         	pipe_list_insert_head($$, $1);
        }
	| pipe SEMICOLON command_line {
		pipe_list_insert_head($3, $1);
		$$ = $3;
	}
	;

pipe:
	command PIPELINE command {
		$$ = command_list_init();
		command_list_insert_head($$, $3);
		command_list_insert_head($$, $1);

	}
	| command PIPELINE pipe {
		command_list_insert_head($3, $1);
		$$ = $3;
	}
	;

redirection:
 	redirection IN_REDIRECTION WORD {
 		$1->in_file = $3;
 		$$ = $1;
	}
	| redirection OUT_REDIRECTION WORD {
		$1->out_file_r = $3;
                $1->out_file_a = NULL;
                $$ = $1;
	}
	| redirection OUT_REDIRECTION OUT_REDIRECTION WORD {
		$$->out_file_r = NULL;
                $$->out_file_a = $4;
                $$ = $1;
        }
	| IN_REDIRECTION WORD {
		$$ = redirection_init();
		$$->in_file = $2;
	}
	| OUT_REDIRECTION WORD {
		$$ = redirection_init();
        	$$->out_file_r = $2;
        	$$->out_file_a = NULL;

	}
	| OUT_REDIRECTION OUT_REDIRECTION WORD {
		$$ = redirection_init();
                $$->out_file_r = NULL;
                $$->out_file_a = $3;
        }
	;

/* An atomic command */
command:
	WORD arguments redirection {
		$$ = command_init();
		$$->command_name = $1;
		$$->argument_list = $2;
		$$->redirection = $3;
	}
	;
	| WORD arguments {
		$$ = command_init();
		$$->command_name = $1;
		$$->argument_list = $2;
	}
	;

arguments: /* Lambda */ { 
		$$ = argument_list_init();
	}
	| arguments WORD { 
		argument *to_add = init_argument();
		to_add->argument_value = $2;
		argument_list_insert_tail($1, to_add);
		$$ = $1;
	}
	| arguments QUOTED { 
		argument *to_add = init_argument();
		to_add->argument_value = $2;
		argument_list_insert_tail($1, to_add);
		$$ = $1;
	}
	;

%%

void yyerror(const char *msg) {
	char * token = strdup(&msg[25]); // Duplicate token's name and on
	char * end = strstr(token, ", expecting"); // Get rid of anything that is behind the name itself
	end[0] = '\0';
	fprintf(stderr, "error:%zu: syntax error near unexpected token '%s'\n", current_line_num, token);
	free(token);
	yyexit_code = 254;
}

/* Restore parser's return value before next run of parser */
void restore_parsers_exit_code(){
	yyexit_code = 0; 
}