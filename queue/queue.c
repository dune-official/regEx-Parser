#include "queue.h"

typedef struct QueueElement {
	struct QueueElement *next;
	unsigned long long element;
} queue_inner;

queue *initialize_queue() {
	queue *aux_node = (queue *) calloc(1, sizeof(*aux_node));
	if (NULL == aux_node) {
		fputs("Could not allocate memory to <<queue *aux_node>>", stderr);
		exit(-1);
	}
	queue_inner *queue_start = NULL;
	queue_inner *queue_end = NULL;
	aux_node->queue_start = queue_start;
	aux_node->queue_end = queue_end;

	return aux_node;
}

void enqueue(unsigned long long to_enqueue, queue* queue) {
	if (NULL == queue->queue_end && NULL == queue->queue_start) {
		queue->queue_end = calloc(1, sizeof(*queue->queue_end));
		if (NULL == queue->queue_end) {
			fputs("Failed to allocate memory to <<queue *queue->queue end>>", stderr);
			exit(-1);
		}
		queue->queue_end->element = to_enqueue;
		queue->queue_start = queue->queue_end;
		queue->queue_end->next = NULL;
	} else {
		/* the edge case here is that the end pointer points to the same element as queue_start */
		queue_inner *queue_walker = calloc(1, sizeof(*queue_walker));
		if (NULL == queue_walker) {
			fputs("Could not allocate memory to <<queue_inner *queue_walker>>", stderr);
			exit(-1);
		}
		queue_walker->element = to_enqueue;
		queue_walker->next = NULL;
		queue->queue_end->next = queue_walker;
		queue->queue_end = queue->queue_end->next;
	}
}

unsigned long long dequeue(queue *queue) {
	if (NULL == queue->queue_end && NULL == queue->queue_start) {
		fputs("Queue is empty", stderr);
		exit(1);
	} else {
		/* now we need to make sure that if the last element is taken, both queue ends are NULL */
		queue_inner *ptr;
		ptr = queue->queue_start;
		unsigned long long to_return = ptr->element;
		if (NULL == queue->queue_start->next) {
			/* meaning, we have only one element in the queue, the queue start is also the queue end */
			queue->queue_start = NULL;
			queue->queue_end = NULL;
		} else {
			queue->queue_start = queue->queue_start->next;
			ptr->next = NULL;
		}
		free(ptr);
		return to_return;
	}
}

unsigned long long peek_queue(queue *queue) {
	if (NULL == queue->queue_end) {
		fputs("Queue is empty", stderr);
		exit(1);
	} else {
		return queue->queue_end->element;
	}
}

char is_empty_queue(queue *queue) {
	if (NULL == queue->queue_end || NULL == queue->queue_start) {
		return 1;
	} else {
		return 0;
	}
}