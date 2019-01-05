%{
    #include <stdio.h>
    #include <string.h>
    #include "data_structures.h"
    void yyerror(const char* msg) {
      fprintf(stderr, "%s\n", msg);
   }
   int yylex();
%}

%union {
	char *str_val;
	int int_val;
    struct commands_handle *commands_handle;
    struct arguments_handle *arguments_handle;
}

/* Declare tokens */ 
%token<str_val> WORD QUOTED
%token<int_val> SEMICOLON END

%type<commands_handle> command commandline
%type<arguments_handle> arguments
%%

/* TODO remove test printings */
commandline: END /* Empty line is ignored */ { YYACCEPT; }
    | command END { 
        /* testing
        struct command *cc;
        struct argument *ca;
        STAILQ_FOREACH(cc,&$1->head,entries){
            printf("cmd: %s args: ",cc->command_name);
            STAILQ_FOREACH(ca,&cc->arguments_handle->head,entries){
                printf("%s ",ca->argument_value);
            }
            printf("\n");
        }*/
        YYACCEPT; 
    } 
    ;

/* Commands on a single line */
/* left recursion -> the commands are chained from behind */
command: WORD arguments  {
        struct command *to_add = init_command();
        to_add->command_name = strdup($1);
        to_add->arguments_handle = $2;  
        $$ = init_command_list();
        command_list_insert_head($$,to_add);
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
        command_list_insert_head($4,to_add);
        $$ = $4;
    } 
    ;

arguments: /* Lambda */ { 
        $$ = init_argument_list();
    }
    | arguments WORD { 
        struct argument *to_add = init_argument();
        to_add->argument_value = $2;
        argument_list_insert_tail($1,to_add);
    }
    | arguments QUOTED { 
        struct argument *to_add = init_argument();
        to_add->argument_value = $2;
        argument_list_insert_tail($1,to_add);
    }
    ;

%%