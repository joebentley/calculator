
#include <stdlib.h>
#include "linked_list.h"

#ifndef STACK_H_
#define STACK_H_

#define STACK_SIZE 1000

// Note: User responsible for freeing malloc'd pointers
typedef struct calc_stack {
	size_t count;
	void *stack[STACK_SIZE];
} calc_stack_t;

calc_stack_t *calc_stack_new();

bool calc_stack_empty(const calc_stack_t *stack);
void *calc_stack_pop(calc_stack_t *stack);
void *calc_stack_top(calc_stack_t *stack);
void calc_stack_push(calc_stack_t *stack, void *val);
linked_list_t *calc_stack_to_list(calc_stack_t *stack);
void calc_stack_destroy(calc_stack_t *stack);

#endif // STACK_H_