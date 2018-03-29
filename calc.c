#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#include "linked_list.h"
#include "stack.h"

int tokenize(char *input, char ***tokens) {
	char *_tokens[1000];
	uint32_t num_tokens = 0;

	char *token = strtok(input, " ");
	_tokens[0] = token;
	token = strtok(NULL, " ");
	for (num_tokens = 1; token; ++num_tokens) {
		_tokens[num_tokens] = token;
		token = strtok(NULL, " ");
	}

	*tokens = malloc(sizeof(char*) * num_tokens);
	memcpy(*tokens, _tokens, sizeof(char*) * num_tokens);

	return num_tokens;
}

bool is_number(const char *str) {
	double a;
	return sscanf(str, "%lf", &a) != 0
		|| !strcmp(str, "pi")
		|| !strcmp(str, "e");
}

typedef struct op_result {
	uint32_t precedence;
	bool left_associative;
	bool exists;
	char *token;
} op_result_t;

op_result_t parse_operator(char *str) {
	op_result_t op_result;
	op_result.left_associative = true;
	op_result.exists = true;

	if (!strcmp(str, "+")) {
		op_result.precedence = 4;
	} else if (!strcmp(str, "-")) {
		op_result.precedence = 4;
	} else if (!strcmp(str, "*")) {
		op_result.precedence = 5;
	} else if (!strcmp(str, "/")) {
		op_result.precedence = 5;
	} else if (!strcmp(str, "^")) {
		op_result.precedence = 6;
	} else if (!strcmp(str, "sin") || !strcmp(str, "cos") || !strcmp(str, "tan")
			|| !strcmp(str, "exp") || !strcmp(str, "exp") || !strcmp(str, "log")
			|| !strcmp(str, "log10") || !strcmp(str, "abs") || !strcmp(str, "atan2")
			|| !strcmp(str, "push") || !strcmp(str, "pop")) {
		op_result.precedence = 7;
		op_result.left_associative = false;
	} else if (strcmp(str, "(") != 0 && strcmp(str, ")") != 0) {
		op_result.exists = false;
	}
	op_result.token = str;
	return op_result;
}

// Shunting-yard algorithm
uint32_t infix_to_rpn(char **tokens, uint32_t n) {
	// char **rpn_tokens = malloc(sizeof(char *) * n);
	uint32_t curr_rpn_token = 0;

	calc_stack_t *op_stack = calc_stack_new();
	// printf("%d\n",is_number(token));

	for (uint32_t i = 0; i < n; ++i) {
		char *token = malloc(sizeof(char) * strlen(tokens[i]));
		strcpy(token, tokens[i]);
		op_result_t *result = malloc(sizeof(op_result_t));
		*result = parse_operator(token);

		if (is_number(token)) {
			tokens[curr_rpn_token++] = token;
		} else if (!strcmp(result->token, "(")) {
			calc_stack_push(op_stack, result);
		} else if (!strcmp(result->token, ")")) {
			if (!calc_stack_empty(op_stack)) {
				op_result_t stack_top = *(op_result_t*)calc_stack_top(op_stack);
				while (strcmp(stack_top.token, "(") != 0) {
					tokens[curr_rpn_token++] = stack_top.token;
					calc_stack_pop(op_stack);
					
					if (calc_stack_empty(op_stack)) {
						fprintf(stderr, "Error: Mismatched parentheses\n");
						exit(-1);
					}

					stack_top = *(op_result_t*)calc_stack_top(op_stack);
				}
				calc_stack_pop(op_stack);
			}
		} else if (result->exists) {
			if (!calc_stack_empty(op_stack)) {
				op_result_t stack_top = *(op_result_t*)calc_stack_top(op_stack);
				while ((stack_top.precedence > result->precedence
					|| (stack_top.precedence == result->precedence && stack_top.left_associative))
					&&  strcmp(stack_top.token, "(") != 0)
				{
					tokens[curr_rpn_token++] = stack_top.token;

					if (op_stack->count > 1) {
						calc_stack_pop(op_stack);
						stack_top = *(op_result_t*)calc_stack_top(op_stack);
					} else {
						calc_stack_pop(op_stack); // pop last element
						break;
					}
				}
			}
			calc_stack_push(op_stack, result);
		}
	}
	while (!calc_stack_empty(op_stack)) {
		op_result_t next_token = *(op_result_t*)calc_stack_pop(op_stack);
		if (!strcmp(next_token.token, "(") || !strcmp(next_token.token, ")")) {
			fprintf(stderr, "Error: Mismatched parentheses\n");
			exit(-1);
		}
		tokens[curr_rpn_token++] = next_token.token;
	}

	// for (uint32_t i = 0; i < curr_rpn_token; ++i) {
	// 	printf("%s\n", tokens[i]);
	// }

	return curr_rpn_token;
}

int parse(char *input, calc_stack_t **ret_stack, bool infix) {
	calc_stack_t *stack = calc_stack_new();
	*ret_stack = stack;
	calc_stack_t *value_stack = calc_stack_new();

	char *tokenizer_string = malloc(sizeof(char) * strlen(input));
	strcpy(tokenizer_string, input);

	char **tokens;
	uint32_t num_tokens = tokenize(input, &tokens);
	uint32_t input_length = num_tokens;

	if (infix) {
		num_tokens = infix_to_rpn(tokens, num_tokens);
	}

	char *token = NULL;
	uint32_t token_num = 0;
	for (; token_num < num_tokens; ++token_num) {
		token = tokens[token_num];
		double *val = malloc(sizeof(double));
		if (sscanf(token, "%lf", val)) {
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "+") == 0) {
			if (stack->count < 2)
				goto stack_err;
			*val = *(double *)calc_stack_pop(stack) + *(double *)calc_stack_pop(stack);
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "-") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val1 = *(double *)calc_stack_pop(stack);
			double val2 = *(double *)calc_stack_pop(stack);
			*val = val2 - val1;
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "*") == 0) {
			if (stack->count < 2)
				goto stack_err;
			*val = *(double *)calc_stack_pop(stack) * *(double *)calc_stack_pop(stack);
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "/") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val1 = *(double *)calc_stack_pop(stack);
			double val2 = *(double *)calc_stack_pop(stack);
			*val = val2 / val1;
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "sqrt") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = sqrt(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "sin") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = sin(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "cos") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = cos(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "tan") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = tan(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "atan2") == 0) {
			if (stack->count < 1)
				goto stack_err;
			double val1 = *(double *)calc_stack_pop(stack);
			double val2 = *(double *)calc_stack_pop(stack);
			*val = atan2(val1, val2);
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "exp") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = exp(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "log") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = log(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "log10") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = log10(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "abs") == 0) {
			if (stack->count < 1)
				goto stack_err;
			*val = fabs(*(double *)calc_stack_pop(stack));
			calc_stack_push(stack, val);
		}
		else if (strcmp(token, "^") == 0) {
			if (stack->count < 2)
				goto stack_err;
			double val1 = *(double *)calc_stack_pop(stack);
			double val2 = *(double *)calc_stack_pop(stack);
			*val = pow(val2, val1);
			calc_stack_push(stack, val);
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
			*val = 4 * atan(1);
			calc_stack_push(stack, &val);
		}
		else if (strcmp(token, "e") == 0) {
			*val = exp(1);
			calc_stack_push(stack, &val);
		}
		else {
			fprintf(stderr, "Error: Invalid token %s\n", token);
		}
	}
	
	free(tokens);
	free(tokenizer_string);
	free(value_stack);
	return 0;

	stack_err: {
		fprintf(stderr, "Stack empty on token %d (%s)\n", token_num, token);

		for (uint32_t i = 0; i < input_length; ++i) {
			fprintf(stderr, "%s ", tokens[i]);
		}
		fprintf(stderr, "\n");

		for (uint32_t i = 0; i < 2 * token_num + 1; ++i) {
			if (i == 2 * token_num)
				fprintf(stderr, "^");
			else
				fprintf(stderr, " ");
		}
		fprintf(stderr, "\n");
		for (uint32_t i = 0; i < 2 * token_num + 1; ++i) {
			if (i == 2 * token_num)
				fprintf(stderr, "|");
			else
				fprintf(stderr, " ");
		}
		fprintf(stderr, "\n");
		for (uint32_t i = 0; i < 2 * token_num + 1; ++i) {
			fprintf(stderr, "-");
		}
		fprintf(stderr, "\n");

		free(tokens);
		free(tokenizer_string);
		free(value_stack);
		return -1;
	}
}

int main(int argc, char *argv[]) {
	char *input;
	bool infix = false;

	if (argc == 1)
		return 1;

	int c;
	while ((c = getopt(argc, argv, "ie:")) != -1) {
		switch (c) {
			case 'e':
				input = optarg;
				break;
			case 'i':
				infix = true;
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
	if (!parse(input, &stack, infix)) {
		linked_list_t *stack_values = calc_stack_flush(stack);
		linked_list_t *begin = stack_values;
		
		printf("Stack:\n");
		while (!ll_end(stack_values)) {
			printf("%lf\n", *(double*)ll_get_and_next(&stack_values));
		}

		free(stack);
		ll_destroy(begin);
		return 0;
	}
	free(stack);	
	return -1;
}
