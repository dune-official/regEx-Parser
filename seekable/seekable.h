#ifndef SEEKABLE_SEEKABLE_H
#define SEEKABLE_SEEKABLE_H

#include <stdio.h>
#include <stdlib.h>

struct SeekableNode {
	void *object;
	struct SeekableNode *next, *previous;
};

typedef struct SeekableWrapper {
	struct SeekableNode *start, *current;
} seek;

extern seek *initialize_seekable();
extern void *peek(seek *node);
extern void *peek_right(seek *node);
extern void *peek_left(seek *node);

extern char seek_right(seek *node);
extern char seek_left(seek *node);

extern void set_current(void *restrict object, struct SeekableNode *restrict node);
extern void set_right(void *restrict object, struct SeekableNode *restrict node);
extern void set_left(void *restrict object, struct SeekableNode *restrict node);
extern void insert_node_right(seek *node);
extern void insert_node_left(seek *node);

#endif