#ifndef QUEUE_QUEUE_H
#define QUEUE_QUEUE_H

#include "../regex/regex.h"

typedef struct AuxiliaryNodeQ {
	struct QueueElement *queue_start, *queue_end;
} queue;

extern void enqueue(unsigned long long to_enqueue, queue *queue);
extern unsigned long long dequeue(queue *queue);
extern char is_empty_queue(queue *queue);
extern queue *initialize_queue();

#endif