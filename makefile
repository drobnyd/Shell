CFLAGS = -g -Wall
LDLIBS = -lreadline -ll
CC = cc
bison_name = grammar
object_files = main.o

lex.yy.c: tokens.l
	flex tokens.l

tokens.l: $(bison_name).tab.h
	
$(bison_name).tab.c: $(bison_name).tab.h

mysh: lex.yy.c $(bison_name).tab.c main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(bison_name).tab.h: $(bison_name).y
	bison -d $(bison_name).y

.PHONY: clean

clean:
	rm main.o mysh