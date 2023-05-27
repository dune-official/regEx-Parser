#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <stdio.h>
#include <stdlib.h>

typedef struct AuxiliaryNodeS {
	struct StackElement *stack_end;
} stack;

extern void push(void *item, stack *stack);
extern void *pop(stack *stack);
extern void *peek_stack(stack *stack);
extern char is_empty_stack(stack *stack);
extern stack *initialize_stack();

#endif
