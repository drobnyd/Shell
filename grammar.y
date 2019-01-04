%{
    #include <stdio.h>
    void yyerror(const char* msg) {
      fprintf(stderr, "%s\n", msg);
   }
   int yylex();
%}

%union {
	char *str_val;
	int int_val;
}

/* Declare tokens */ 
%token<str_val> WORD QUOTED
%token<int_val> SEMICOLON NEWLINE

%type<str_val> arguments
%%

/* TODO printings */
commandline: NEWLINE /* Empty line is ignored */ {printf("x"); YYACCEPT; }
    | command NEWLINE { printf("\n"); YYACCEPT; } 
    ;

/* Commands on a single line */
command: cmd arguments
    | cmd arguments SEMICOLON { printf(";"); } 
    | cmd arguments SEMICOLON command { printf(";"); } 

cmd: WORD { printf("%s ", $1); }
    ;

arguments: /* Lambda */
    | arguments WORD { printf("%s ", $2); }
    | arguments QUOTED { printf("%s ", $2); }
    ;

%%