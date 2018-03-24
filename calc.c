#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#include "linked_list.h"

#define STACK_SIZE 1000

typedef struct calc_stack {
	size_t count;
	double stack[STACK_SIZE];
} calc_stack_t;

calc_stack_t *calc_stack_new() {
	calc_stack_t *stack = malloc(sizeof(calc_stack_t));
	stack->count = 0;
	return stack;
}

bool calc_stack_empty(const calc_stack_t *stack) {
	return stack->count == 0;
}

double calc_stack_pop(calc_stack_t *stack) {
	return stack->stack[--stack->count];
}

void calc_stack_push(calc_stack_t *stack, double val) {
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

int parse(char *input, calc_stack_t **ret_stack) {
	char *token;

	calc_stack_t *stack = calc_stack_new();
	*ret_stack = stack;
	calc_stack_t *value_stack = calc_stack_new();
	uint32_t token_num = 0;

	char *tokenizer_string = malloc(sizeof(char) * strlen(input));
	strcpy(tokenizer_string, input);
	token = strtok(tokenizer_string, " ");
	while (token) {
		double a;
		if (sscanf(token, "%lf", &a)) {
			calc_stack_push(stack, a);
		}
		else if (strcmp(token, "+") == 0) {
			if (stack->count < 2)
				goto stack_err;
			calc_stack_push(stack, calc_stack_pop(stack) + calc_stack_pop(stack));
		}
		else if (strcmp(token, "-") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val = calc_stack_pop(stack);
			calc_stack_push(stack, calc_stack_pop(stack) - val);
		}
		else if (strcmp(token, "*") == 0) {
			if (stack->count < 2)
				goto stack_err;
			calc_stack_push(stack, calc_stack_pop(stack) * calc_stack_pop(stack));
		}
		else if (strcmp(token, "/") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val = calc_stack_pop(stack);
			calc_stack_push(stack, calc_stack_pop(stack) / val);
		}
		else if (strcmp(token, "sqrt") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, sqrt(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "sin") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, sin(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "cos") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, cos(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "tan") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, tan(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "exp") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, exp(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "log") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, log(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "log10") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, log10(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "abs") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, fabs(calc_stack_pop(stack)));
		}
		else if (strcmp(token, "pow") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val = calc_stack_pop(stack);
			calc_stack_push(stack, pow(calc_stack_pop(stack), val));
		}
		/* stack functions */
		else if (strcmp(token, "push") == 0) {
			if (stack->count < 1)
				goto stack_err;
			calc_stack_push(value_stack, calc_stack_pop(stack));
		}
		else if (strcmp(token, "pop") == 0) {
			if (value_stack->count < 1)
				goto stack_err;
			calc_stack_push(stack, calc_stack_pop(value_stack));
		}
		/* constants */
		else if (strcmp(token, "pi") == 0) {
			calc_stack_push(stack, 4 * atan(1));
		}
		else if (strcmp(token, "e") == 0) {
			calc_stack_push(stack, exp(1));
		}
		else {
			fprintf(stderr, "Error: Invalid token\n");
		}

		token = strtok(NULL, " ");
		++token_num;
	}
	
	free(tokenizer_string);
	free(value_stack);
	return 0;

	stack_err: {
		fprintf(stderr, "Stack empty on token %d (%s)\n", token_num, token);
		fprintf(stderr, "%s\n", input);

		uint32_t linewidth = token - tokenizer_string;

		for (uint32_t i = 0; i < linewidth + 1; ++i) {
			if (i == linewidth)
				fprintf(stderr, "^");
			else
				fprintf(stderr, " ");
		}
		fprintf(stderr, "\n");
		for (uint32_t i = 0; i < linewidth + 1; ++i) {
			if (i == linewidth)
				fprintf(stderr, "|");
			else
				fprintf(stderr, " ");
		}
		fprintf(stderr, "\n");
		for (uint32_t i = 0; i < linewidth + 1; ++i) {
			fprintf(stderr, "-");
		}
		fprintf(stderr, "\n");
		return -1;
	}
}

int main(int argc, char *argv[]) {
	char *input;

	// if (argc == 1) {
	// 	char str[1000];
	// 	size_t bytes_read = fread(str, sizeof(char), 1000, stdin);
	// 	// trim newline
	// 	if (str[bytes_read - 1] == '\n')
	// 		str[bytes_read - 1] = '\0';
	// 	input = str;
	// } else if (strcmp(argv[1], "-e") == 0 && argc == 3) {
	// 	input = argv[2];
	// } else {
	// 	fprintf(stderr, "Usage: calc -e [input]\n");
	// 	return -1;
	// }

	if (argc == 1)
		return 1;

	int c;
	while ((c = getopt(argc, argv, "e:")) != -1) {
		switch (c) {
			case 'e':
				input = optarg;
				break;
			case '?':
				if (optopt == 'e')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort();
		}
	}

	calc_stack_t *stack = NULL;
	if (!parse(input, &stack)) {
		linked_list_t *stack_values = calc_stack_flush(stack);
		linked_list_t *begin = stack_values;
		
		printf("Stack:\n");
		while (!ll_end(stack_values)) {
			printf("%lf\n", ll_get_and_next(&stack_values));
		}

		free(stack);
		ll_destroy(begin);
		return 0;
	}
	free(stack);	
	return -1;
}
