.DEFAULT_GOAL = mysh
CFLAGS = -g -Wall -Wpedantic
CC = cc
BISON = grammar
O_FILES = command_execution.o data_structures.o run_modes.o parser_caller.o internal_commands.o utils.o 
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

mysh: lex.yy.c $(BISON).tab.c main.o  $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

run: mysh
	./mysh

test_mysh: lex.yy.c $(BISON).tab.c test_main.o $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(BISON).tab.h: $(BISON).y
	bison -d $(BISON).y

.PHONY: clean cstyle tests

tests: mysh test_mysh
	bash phase1_tests.sh

cstyle: main.c command_execution.h command_execution.c data_structures.c data_structures.h run_modes.c parser_caller.c internal_commands.c utils.c run_modes.h parser_caller.h internal_commands.h utils.h
	../cstyle.pl main.c command_execution.h command_execution.c data_structures.c data_structures.h run_modes.c parser_caller.c internal_commands.c utils.c run_modes.h parser_caller.h internal_commands.h utils.h

clean:
	rm $(O_FILES) main.o test_main.o test_mysh mysh lex.yy.c $(BISON).tab.c $(BISON).tab.h
