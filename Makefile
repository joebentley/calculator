CC=gcc
CFLAGS=-Wall -Werror -g

DEPS=linked_list.h stack.h
OBJ=calc.o linked_list.o stack.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

calc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

clean:
	rm -rf *.o calc calc.dSYM
