#include "regex.h"
#include "stack.h"

_Bool can_merge(char stack_char, char input_char) {
	if (stack_char < 2 || stack_char > 4) {
		return false;
	}

	if (stack_char >= input_char) {
		return true;
	}
	return false;
}

void merge_union(stack *op_stack) {
	regexNode *RHS = (regexNode *) pop(op_stack);
	regexNode *LHS = (regexNode *) pop(op_stack);

	if (NULL == RHS || NULL == LHS) {
		fputs("Parsing error - please check your input", stderr);
		exit(1);
	}

	regexNode *ptr = union_re(LHS, RHS);
	push((void *) ptr, op_stack);
}

void merge_concat(stack *op_stack) {
	regexNode *RHS = (regexNode *) pop(op_stack);
	regexNode *LHS = (regexNode *) pop(op_stack);

	if (NULL == RHS || NULL == LHS) {
		fputs("Parsing error - please check your input", stderr);
		exit(1);
	}

	regexNode *ptr = concat(LHS, RHS);
	push((void *) ptr, op_stack);
}

void merge_kleene(stack *op_stack) {
	regexNode *LHS = (regexNode *) pop(op_stack);

	if (NULL == LHS) {
		fputs("Parsing error - please check your input", stderr);
		exit(1);
	}

	regexNode *ptr = kleene(LHS);
	push((void *) ptr, op_stack);
}

regexNode *parse(char *string) {
	int i;
	char lookahead, *top_ptr;
	regexNode *ptr;

	stack *operator_stack = initialize_stack();
	stack *operand_stack = initialize_stack();

	/* push a sentinel value on top of the stack to not unexpectedly run into any problems */
	top_ptr = (char *) calloc(1, sizeof(*top_ptr));
	if (NULL == top_ptr) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}
	top_ptr[0] = '\0';
	push((void *) top_ptr, operand_stack);

	unsigned long long strl = strnlen(string, 256);

	for (i = 0; i < strl; i++) {
		lookahead = string[i + 1];

		top_ptr = (char *) calloc(1, sizeof(*top_ptr));
		if (NULL == top_ptr) {
			fputs("Failed to init buffer", stderr);
			exit(1);
		}

		switch (string[i]) {
			case '|':
				if (!can_merge(*(char *) peek_stack(operand_stack), UNION)) {
					top_ptr[0] = UNION;
					push((void *) top_ptr, operand_stack);
				} else {
					merge_union(operator_stack);
				}

				if (lookahead == ')' || lookahead == '|') {
					ptr = symbol(EPSILON);
					push((void *) ptr, operator_stack);
				}
				break;

			case '*':
				if (!can_merge(*(char *) peek_stack(operand_stack), KLEENE)) {
					top_ptr[0] = KLEENE;
					push((void *) top_ptr, operand_stack);
				} else {
					merge_kleene(operator_stack);
				}

				/* lookahead is not a special char */
				switch (lookahead) {
					case ')':
					case '|':
					case '\0':
					case '*':
						free(top_ptr);
						break;
					case '(':
					default:
						if (!can_merge(*(char *) peek_stack(operand_stack), CONCAT)) {
							/* push onto the OPERAND stack */
							top_ptr[0] = CONCAT;
							push((void *) top_ptr, operand_stack);
						} else {
							merge_concat(operator_stack);
						}
				}

				break;

			case '(':
				top_ptr[0] = '(';
				push((void *) top_ptr, operand_stack);
				if (lookahead == '|') {
					ptr = symbol(EPSILON);
					push((void *) ptr, operator_stack);
				}
				break;

			case ')':
				if (is_empty_stack(operand_stack)) {
					fputs("Parsing error - please check your input", stderr);
					exit(1);
				}
				while (*(top_ptr = (char *) pop(operand_stack)) != '(') {
					switch (*top_ptr) {
						case KLEENE:
							merge_kleene(operator_stack);
							break;
						case CONCAT:
							merge_concat(operator_stack);
							break;
						case UNION:
							merge_union(operator_stack);
							break;
						default:
							fputs("This should not have happened - panic mode", stderr);
							exit(1);
					}
					free(top_ptr);
				}

				/* lookahead is not a special char */
				switch (lookahead) {
					case ')':
					case '|':
					case '\0':
					case '*':
						break;
					case '(':
					default:
						top_ptr[0] = CONCAT;
						push((void *) top_ptr, operand_stack);
				}
				break;

			case '\\':
				i++;
				lookahead = string[i + 1];

			default:
				ptr = symbol(string[i]);
				push((void *) ptr, operator_stack);
				/* lookahead is not a special char */
				switch (lookahead) {
					case ')':
					case '|':
					case '\0':
					case '*':
						free(top_ptr);
						break;
					case '(':
					default:
						if (!can_merge(*(char *) peek_stack(operand_stack), CONCAT)) {
							/* push onto the OPERAND stack */
							top_ptr[0] = CONCAT;
							push((void *) top_ptr, operand_stack);
						} else {
							merge_concat(operator_stack);
						}
				}
		}
	}

	/* now that the whole string is consumed, we need to empty the operand stack */
	while (*(top_ptr = (char *) pop(operand_stack)) != '\0') {
		switch (*top_ptr) {
			case KLEENE:
				merge_kleene(operator_stack);
				break;
			case CONCAT:
				merge_concat(operator_stack);
				break;
			case UNION:
				merge_union(operator_stack);
				break;
			default:
				fputs("This should not have happened - panic mode", stderr);
				exit(1);
		}
		free(top_ptr);
	}

	return (regexNode *) pop(operator_stack);
}
