CC = cc
bison_name = grammar
object_files = main.o

lex.yy.c: tokens.l
	flex tokens.l

tokens.l: $(bison_name).tab.h
	
$(bison_name).tab.c: $(bison_name).tab.h

%.o: %.c
	$(CC) $(CFLAGS) -c $<

mysh: lex.yy.c $(bison_name).tab.c $(object_files)
	$(CC) -o $@ $^ -ll

$(bison_name).tab.h: $(bison_name).y
	bison -d $(bison_name).y

.PHONY: clean

clean: 
	rm mysh