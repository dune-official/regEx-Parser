#include "../../header/parser.h"

/* This function initializes a seekable data structure.
 * A seekable is a structure that contains two pointers to a doubly linked list. The first one points to the start of
 *  the linked list and the second one to the current position (aka. "seek header"). The user is able to:
 *  - seekable_peek the position of and to the right of the seek header
 *  - seek the header right by one
 *  - insert an element to the right of the seek header
 * Output:
 * - seek *root: The root node of the data structure;
 */
seek *seekable_initialize() {
	NEW(seek, root, 1)
    NEWSTRUCT(struct SeekableNode, root->start)

    root->current = root->start;

	/* initializing the sentinel nodes right and left of the current position */
	/* todo: Dear coder, I urge you NOT to move by those sentinel nodes, for all that is beloved and holy */

    NEWSTRUCT(struct SeekableNode, root->start->previous)
    root->start->previous->next = root->start;

    NEWSTRUCT(struct SeekableNode, root->start->next)
    root->start->next->previous = root->start;

	return root;
}

/* This function returns the element at the current position without changing the position.
 * Input:
 * - seek *node: The relevant root structure;
 * Output:
 * - void *current: The current object / NULL;
 */
void *seekable_peek(seek *node) {
	if (NULL != node->current->object) {
		return node->current->object;
	}
	return NULL;
}

/* This function returns the element right to the current position without changing the position.
 * Output:
 * - void *right: The right object / NULL;
 */
void *seekable_peek_right(seek *node) {
	if (NULL != node->current->next && NULL != node->current->next->object) {
		return node->current->next->object;
	}
	return NULL;
}

/* This function moves the current object one to the right.
 * Input:
 * - seek *node: The relevant root structure;
 * Output:
 * - char success: 0 if the seek was successful, -1 otherwise;
 */
char seekable_seek_right(seek *node) {
	if (NULL != node->current->next) {
		node->current = node->current->next;
		return 0;
	}
	return -1;
}

/* This function sets the current SeekableNode.
 * Input:
 * - void *restrict object: The object to be set;
 * - struct SeekableNode: The node that should be set;
 */
void seekable_set_current(void *restrict object, struct SeekableNode *restrict node) {
	if (NULL != node->object) {
		free(node->object);
	}
	node->object = object;
}

/* This function sets the SeekableNode to the right of the seek header. Used in combination of "seekable_insert_node_right".
 * Input:
 * - void *restrict object: The object to be set;
 * - struct SeekableNode: The node that should be set;
 */
void seekable_set_right(void *restrict object, struct SeekableNode *restrict node) {
	if (NULL != node->next) {
        seekable_set_current(object, node->next);
	}
}

/* This function inserts a SeekableNode right to the seek header. The node is empty, meaning
 *  the programmer has to seek right by one and then use the "seekable_set_current" or simply "seekable_set_right".
 * Input:
 * - seek *node: The relevant root structure;
 */
void seekable_insert_node_right(seek *node) {
	NEW(struct SeekableNode, ptr, 1)

	if (NULL != node->current->next) {
		struct SeekableNode *ptr2 = node->current->next;
		ptr->next = ptr2;
		ptr2->previous = ptr;
	}

	ptr->previous = node->current;
	node->current->next = ptr;
}