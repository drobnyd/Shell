%{
	#include <stdio.h>
	#include <string.h>
	#include "data_structures.h"
	void yyerror(const char *msg);
	int yylex();
	struct pipe_handle *parsed_commands = NULL;
	extern size_t current_line_num;
	size_t yyexit_code = 0;
%}

%union {
	char * str_val;
	int int_val;
	struct commands_handle *commands_handle;
	struct arguments_handle *arguments_handle;
	struct pipe_handle *pipe_handle;
	struct command *command_ptr;
}

%error-verbose

/* Declare tokens */ 
%token<str_val> WORD "word"
%token<str_val> QUOTED "quote"
%token SEMICOLON ";"
%token PIPELINE "|"
%token END "EOF"
%token EOL "\\n"

%destructor { free($$); } WORD QUOTED
%destructor { deallocate_command($$); } command
%destructor { deallocate_arguments($$); } arguments
%destructor { deallocate_commands($$); } pipe
%destructor { deallocate_pipe($$); } command_line

%type<commands_handle> pipe
%type<arguments_handle> arguments
%type<command_ptr> command
%type<pipe_handle> command_line
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
		$$ = init_pipe_list();
		pipe_list_insert_simple_head($$, $1);
	}
	| command SEMICOLON { 
		$$ = init_pipe_list();
		pipe_list_insert_simple_head($$, $1);
	} 
	| command SEMICOLON command_line {
		pipe_list_insert_simple_head($3, $1);
		$$ = $3;
	}
	| pipe {
		$$ = init_pipe_list();
		pipe_list_insert_head($$, $1);
	}
	| pipe SEMICOLON command_line {
		pipe_list_insert_head($3, $1);
		$$ = $3;
	}
	;

pipe:
	command PIPELINE command {
		$$ = init_command_list();
		command_list_insert_head($$, $3);
		command_list_insert_head($$, $1);

	}
	| command PIPELINE pipe {
		command_list_insert_head($3, $1);
		$$ = $3;
	}
	;

/* An atomic command */
command: WORD arguments {
		$$ = init_command();
		$$->command_name = $1;
		$$->arguments_handle = $2;
	}
	;

arguments: /* Lambda */ { 
		$$ = init_argument_list();
	}
	| arguments WORD { 
		struct argument *to_add = init_argument();
		to_add->argument_value = $2;
		argument_list_insert_tail($1, to_add);
		$$ = $1;
	}
	| arguments QUOTED { 
		struct argument *to_add = init_argument();
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