#include "../../header/parser.h"

typedef struct QueueElement {
	struct QueueElement *next;
	unsigned long long element;
} queue_inner;

/* This function initializes a queue.
 * Output:
 * - queue *aux_node: The root of the queue;
 */
queue *queue_initialize() {
	NEW(queue, aux_node, 1)

	queue_inner *queue_start = NULL;
	queue_inner *queue_end = NULL;
	aux_node->queue_start = queue_start;
	aux_node->queue_end = queue_end;

	return aux_node;
}

/* This function enqueues an element.
 * Input:
 * - unsigned long long to_enqueue: The element to add to the queue:
 * - queue *queue: The root of the queue;
 */
void queue_enqueue(unsigned long long to_enqueue, queue* queue) {
	if (NULL == queue->queue_end && NULL == queue->queue_start) {
        NEWSTRUCT(struct QueueElement, queue->queue_end)

		queue->queue_end->element = to_enqueue;
		queue->queue_start = queue->queue_end;
		queue->queue_end->next = NULL;
	} else {
		/* the edge case here is that the end pointer points to the same element as queue_start */
        NEW(queue_inner, queue_walker, 1)

		queue_walker->element = to_enqueue;
		queue_walker->next = NULL;
		queue->queue_end->next = queue_walker;
		queue->queue_end = queue->queue_end->next;
	}
}

/* This function dequeues an element.
 * Input:
 * - queue *queue: The root of the queue;
 * Output:
 * - unsigned long long to_return: The next element in line;
 */
unsigned long long queue_dequeue(queue *queue) {
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

/* This function checks if the queue is empty or not.
 * Input:
 * - queue *queue: The root of the queue;
 * Output:
 * - bool is_empty: true if the queue is empty, false otherwise;
 */
bool queue_is_empty(queue *queue) {
	if (NULL == queue->queue_end || NULL == queue->queue_start) {
		return 1;
	} else {
		return 0;
	}
}