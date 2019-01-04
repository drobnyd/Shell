

extern void yyparse(void);
extern void yy_scan_string(char*);
extern void yylex_destroy(void);

int
main(int argc, char **argv)
{
    yy_scan_string("ahoj;\n");
	yyparse();
	yylex_destroy();
}