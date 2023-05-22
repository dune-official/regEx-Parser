#include "stack.h"

typedef struct StackElement {
	struct StackElement *next;
	void *element;
} stack_inner;

stack *initialize_stack() {
	stack *aux_node = (stack *) calloc(1, sizeof(*aux_node));
	if (NULL == aux_node) {
		fputs("Could not allocate memory to <<stack *aux_node>>", stderr);
		exit(-1);
	}
	stack_inner *stack_end = NULL;
	aux_node->stack_end = stack_end;
	return aux_node;
}

void push(void *to_push, stack *stack) {
	if (NULL == stack->stack_end) {
		stack->stack_end = (stack_inner *) calloc(1, sizeof(*stack->stack_end));
		if (NULL == stack->stack_end) {
			fputs("Could not allocate memory for <<stack *stack->stack_end>>", stderr);
			exit(-1);
		}
		stack->stack_end->next = NULL;
		stack->stack_end->element = to_push;
	} else {
		stack_inner *stack_walker = (stack_inner *) calloc(1, sizeof(*stack_walker));
		if (NULL == stack_walker) {
			fputs("Could not allocate memory for <<stack_inner *stack_walker>>", stderr);
			exit(-1);
		}
		stack_walker->element = to_push;
		stack_walker->next = stack->stack_end;
		stack->stack_end = stack_walker;
	}
}

void *pop(stack *stack) {
	if (NULL == stack->stack_end) {
		/* this means the stack has already been purged */
		return NULL;
	} else {
		void *to_return = stack->stack_end->element;
		stack_inner *ptr = stack->stack_end;
		stack->stack_end = stack->stack_end->next;
		ptr->next = NULL;
		free(ptr);
		return to_return;
	}
}

void *peek_stack(stack *stack) {
	if (NULL == stack->stack_end) {
		return NULL;
	} else {
		return stack->stack_end->element;
	}
}

char is_empty_stack(stack *stack) {
	if (NULL == stack->stack_end) {
		return 1;
	} else {
		return 0;
	}
}