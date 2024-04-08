/*
Name: Divya Gariga, Vaishnavi Panchalingala
BlazerId: DGARIGA, vpancha2
Project #: HW4
To compile: gcc queue.c -pthread ./mysched.c
program> To run: ./a.out 2
*/


/* Implementation of a simple circular queue using a static array */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>

/* create the queue data structure and initialize it */
queue *queue_init(int n) {
	queue *q = (queue *)malloc(sizeof(queue));
	q->size = n;
	q->buffer = malloc(sizeof(int)*n);
	q->start = 0;
	q->end = 0;
	q->count = 0;

	return q;
}

/* insert an item into the queue, update the pointers and count, and
   return the no. of items in the queue (-1 if queue is null or full) */
int queue_insert(queue *q, const Job* item) {
	if ((q == NULL) || (q->count == q->size))
	   return -1;

q->buffer[q->end % q->size] = (Job*)item;
	q->end = (q->end + 1) % q->size;
	q->count++;

	return q->count;
}

/* delete an item from the queue, update the pointers and count, and 
   return the item deleted (-1 if queue is null or empty) */
Job* queue_delete(queue *q) {
	if ((q == NULL) || (q->count == 0))
	   return NULL;

	Job *deleted_item = q->buffer[q->start];
    q->start = (q->start + 1) % q->size;
    q->count--;

    return deleted_item;
}

/* display the contents of the queue data structure */
void queue_display(queue *q) {
	int i;
	if (q != NULL && q->count != 0) {
		printf("queue has %d elements, start = %d, end = %d\n", 
			q->count, q->start, q->end);
		printf("queue contents: ");
		for (i = 0; i < q->count; i++){
			  Job *current_job = q->buffer[(q->start + i) % q->size];
            printf("Job ID: %d, Command: %s, Status: %s\n",
                current_job->job_id, current_job->command, current_job->status);
		}
	} else
		printf("queue empty, nothing to display\n");
}

/* delete the queue data structure */
void queue_destroy(queue *q) {
	free(q->buffer);
	free(q);
}

int queue_empty(queue *q) {
    return (q == NULL || q->count == 0);
}

