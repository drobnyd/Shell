%{
    #include <stdio.h>
    #include <string.h>
    #include "data_structures.h"
    void yyerror(const char *msg);
    int yylex();
    struct commands_handle *parsed_commands = NULL;
    extern size_t current_line_num;
    size_t yyexit_value = 0;
%}

%union {
	char * str_val;
	int int_val;
    struct commands_handle *commands_handle;
    struct arguments_handle *arguments_handle;
}

%error-verbose

/* Declare tokens */ 
%token<str_val> WORD "word"
%token<str_val> QUOTED "quote"
%token<int_val> SEMICOLON ";"
%token END "EOF"
%token EOL "\\n"

%type<commands_handle> command commandline
%type<arguments_handle> arguments
%%

commandline: end /* Empty line is ignored */ { parsed_commands = NULL; YYACCEPT; }
    | command end { 
        parsed_commands = $1;
        YYACCEPT; 
    } 
    ;

end: EOL 
    | END
    ;

/* Commands on a single line */
/* left recursion -> the commands are chained from behind */
command: WORD arguments  {
        struct command *to_add = init_command();
        to_add->command_name = strdup($1);
        to_add->arguments_handle = $2;
        $$ = init_command_list();
        command_list_insert_head($$, to_add);
    }
    | WORD arguments SEMICOLON { 
        struct command *to_add = init_command();
        to_add->command_name = strdup($1);
        to_add->arguments_handle = $2;  
        $$ = init_command_list();
        command_list_insert_head($$,to_add);
    } 
    | WORD arguments SEMICOLON command {
        struct command *to_add = init_command();
        to_add->command_name = strdup($1);
        to_add->arguments_handle = $2;
        command_list_insert_head($4, to_add);
        $$ = $4;
    } 
    ;

arguments: /* Lambda */ { 
        $$ = init_argument_list();
    }
    | arguments WORD { 
        struct argument *to_add = init_argument();
        to_add->argument_value = $2;
        argument_list_insert_tail($1, to_add);
    }
    | arguments QUOTED { 
        struct argument *to_add = init_argument();
        to_add->argument_value = $2;
        argument_list_insert_tail($1, to_add);
    }
    ;

%%

void yyerror(const char * msg) {
    char * token = strdup(&msg[25]); // Duplicate token's name and on
    char * end = strstr(token, ", expecting"); // Get rid of anything that is behind the name itself
    end[0] = '\0';
    fprintf(stderr, "error:%zu: syntax error near unexpected token '%s'\n", current_line_num, token);
    free(token);
    yyexit_value = 254;
}

/* Restore parser's return value before next run of parser */
void restore_exit_value(){
    yyexit_value = 0; 
}