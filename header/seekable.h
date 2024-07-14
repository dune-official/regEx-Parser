#ifndef SEEKABLE_SEEKABLE_H
#define SEEKABLE_SEEKABLE_H

#include "parser.h"

extern seek *seekable_initialize();
extern void *seekable_peek(seek *node);
extern void *seekable_peek_right(seek *node);

extern char seekable_seek_right(seek *node);

extern void seekable_set_current(void *restrict object, struct SeekableNode *restrict node);
extern void seekable_set_right(void *restrict object, struct SeekableNode *restrict node);
extern void seekable_insert_node_right(seek *node);

#endif