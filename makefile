CFLAGS = -g -Wall -Wpedantic
LDLIBS = -lreadline -ll
CC = cc
bison_name = grammar
object_files = main.o command_execution.o data_structures.o run_modes.o parser_caller.o

lex.yy.c: tokens.l
	flex tokens.l

tokens.l: $(bison_name).tab.h
	
$(bison_name).tab.c: $(bison_name).tab.h

mysh: lex.yy.c $(bison_name).tab.c $(object_files)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(bison_name).tab.h: $(bison_name).y
	bison -d $(bison_name).y

.PHONY: clean

clean:
	rm $(object_files) mysh