CFLAGS = -g -Wall -Wpedantic
CC = cc
BISON = grammar
O_FILES = main.o command_execution.o data_structures.o run_modes.o parser_caller.o internal_commands.o utils.o 
OS = $(shell uname)
ifeq ($(OS),Darwin)
	FLEX_LIB = -ll
else
	FLEX_LIB = -lfl
endif
LDLIBS = -lreadline $(FLEX_LIB)

lex.yy.c: tokens.l
	flex tokens.l

tokens.l: $(BISON).tab.h
	
$(BISON).tab.c: $(BISON).tab.h

mysh: lex.yy.c $(BISON).tab.c $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(BISON).tab.h: $(BISON).y
	bison -d $(BISON).y

.PHONY: clean

clean:
	rm $(O_FILES) mysh
