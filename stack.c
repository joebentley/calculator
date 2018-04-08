#include "stack.h"

calc_stack_t *calc_stack_new() {
	calc_stack_t *stack = malloc(sizeof(calc_stack_t));
	stack->count = 0;
	return stack;
}

bool calc_stack_empty(const calc_stack_t *stack) {
	return stack->count == 0;
}

void *calc_stack_pop(calc_stack_t *stack) {
	// free(stack->stack[stack->count - 1]);
	return stack->stack[--stack->count];
}

void *calc_stack_top(calc_stack_t *stack) {
	return stack->stack[stack->count - 1];
}

void calc_stack_push(calc_stack_t *stack, void *val) {
	stack->stack[stack->count] = val;
	stack->count++;
}

linked_list_t *calc_stack_to_list(calc_stack_t *stack) {
	linked_list_t *ll = ll_new();
	linked_list_t *begin = ll;
	for (size_t i = 0; i < stack->count; ++i) {
		ll_push(&ll, stack->stack[stack->count - i - 1]);
	}

	return begin;
}

void calc_stack_destroy(calc_stack_t *stack) {
	for (size_t i = 0; i < stack->count; ++i) {
		free(stack->stack[i]);
	}

	free(stack);
}