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
	return stack->stack[--stack->count];
}

void *calc_stack_top(calc_stack_t *stack) {
	return stack->stack[stack->count - 1];
}

void calc_stack_push(calc_stack_t *stack, void *val) {
	stack->stack[stack->count] = val;
	stack->count++;
}

linked_list_t *calc_stack_flush(calc_stack_t *stack) {
	if (stack->count == 0) {
		return NULL;
	}

	linked_list_t *ll = ll_new();
	linked_list_t *begin = ll;
	for (size_t i = 0; stack->count > 0; ++i) {
		ll_push(&ll, calc_stack_pop(stack));
	}

	return begin;
}