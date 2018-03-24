CC=gcc
CFLAGS=-Wall -Werror -g

all:
	$(CC) $(CFLAGS) calc.c linked_list.c -o calc
