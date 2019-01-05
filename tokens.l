%{
    #include <stdio.h>
    #include "grammar.tab.h"
%}

WHITESPACE[ \r\t\f\v]
WORD[^'";# \t\n\r\f\v]+

/* States when special characters are not that special anymore */
%x SINGLE_QUOTES
%x DOUBLE_QUOTES
/* Ignore the contents until end od line */
%x COMMENT

%%

<SINGLE_QUOTES>"\\\'" { /* TODO Escaped double quotes with \' append ' to command*/
    strcat(yylval.str_val,"\'");
}

<DOUBLE_QUOTES>"\\\"" { /* TODO Escaped double quotes with \" append " to command*/
    strcat(yylval.str_val, "\"");
}

<SINGLE_QUOTES>"'" {
    BEGIN(INITIAL);
    return QUOTED;
}

<DOUBLE_QUOTES>\" {
    BEGIN(INITIAL);
    return QUOTED;
}

    /* TODO line counting */
<COMMENT>"\n" {
    BEGIN(INITIAL); /* Comment is ended by new line */
}

<COMMENT><<EOF>> {
    BEGIN(INITIAL); /* Comment is ended by new line */
    return END;
}

<COMMENT>. /* Do nothing, the comment is not passed any further */

<SINGLE_QUOTES,DOUBLE_QUOTES>. {
   strcat(yylval.str_val,yytext);
}

{WORD} {
    yylval.str_val = strdup(yytext);
    return WORD;
}

";" {
    return SEMICOLON; 
}

<<EOF>>   {
    return END;
}

"#" {
    BEGIN(COMMENT);
}  

"'" {
    BEGIN(SINGLE_QUOTES);
    yylval.str_val = (char*)(malloc(sizeof(char)));
    yylval.str_val[0] = '\0';
}   

\"  {
    BEGIN(DOUBLE_QUOTES);
    yylval.str_val = (char*)(malloc(sizeof(char)));
    yylval.str_val[0] = '\0';
}

{WHITESPACE}+   /* Ignore whitespaces */

%%
