#include "seekable.h"

seek *initialize_seekable() {
	seek *toReturn = (seek *) calloc(1, sizeof(*toReturn));
	if (NULL == toReturn) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}

    toReturn->start = (struct SeekableNode *) calloc(1, sizeof(*toReturn->start));
	if (NULL == toReturn->start) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}
    toReturn->current = toReturn->start;

	/* initializing the sentinel nodes right and left of the current position */
	/* todo: Dear coder, I urge you NOT to move by those sentinel nodes, for all that is beloved and holy */

    toReturn->start->previous = (struct SeekableNode *) calloc(1, sizeof(*toReturn->start->previous));
	if (NULL == toReturn->start->previous) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}
    toReturn->start->previous->next = toReturn->start;

    toReturn->start->next = (struct SeekableNode *) calloc(1, sizeof(*toReturn->start->next));
	if (NULL == toReturn->start->previous) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}
    toReturn->start->next->previous = toReturn->start;

	return toReturn;
}

void *peek(seek *node) {
	if (NULL != node->current->object) {
		return node->current->object;
	}
	return NULL;
}

void *peek_right(seek *node) {
	if (NULL != node->current->next && NULL != node->current->next->object) {
		return node->current->next->object;
	}
	return NULL;
}

char seek_right(seek *node) {
	if (NULL != node->current->next) {
		node->current = node->current->next;
		return 0;
	}
	return -1;
}

void set_current(void *restrict object, struct SeekableNode *restrict node) {
	if (NULL != node->object) {
		free(node->object);
	}
	node->object = object;
}

void set_right(void *restrict object, struct SeekableNode *restrict node) {
	if (NULL != node->next) {
		set_current(object, node->next);
	}
}

void insert_node_right(seek *node) {
	struct SeekableNode *ptr = calloc(1, sizeof(*ptr));
	if (NULL == ptr) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}

	if (NULL != node->current->next) {
		struct SeekableNode *ptr2 = node->current->next;
		ptr->next = ptr2;
		ptr2->previous = ptr;
	}

	ptr->previous = node->current;
	node->current->next = ptr;
}