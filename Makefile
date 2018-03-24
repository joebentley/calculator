CC=gcc
CFLAGS=-Wall -Werror -g -lm

all:
	$(CC) $(CFLAGS) calc.c linked_list.c stack.c -o calc
