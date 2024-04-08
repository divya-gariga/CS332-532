/*
Name: Divya Gariga, Vaishnavi Panchalingala
BlazerId: DGARIGA, vpancha2
Project #: HW4
To compile: gcc queue.c -pthread ./dgariga_and_vpancha2_HW04.c
program> To run: ./a.out 2
*/

/* Header file for the simple circular queue example */
#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct job {
    int job_id;
    pthread_t tid;  /* thread ID */
    char *command;
    char *arguments;
    char *status;
    time_t start_time;
    time_t end_time;
} Job;

typedef struct _queue {
	int size;    /* maximum size of the queue */
	Job **buffer; /* queue buffer */
	int start;   /* index to the start of the queue */
	int end;     /* index to the end of the queue */
	int count;   /* no. of elements in the queue */
} queue;

queue *queue_init(int n);
int queue_insert(queue *q, const Job* item);
Job* queue_delete(queue *q);
void queue_display(queue *q);
void queue_destroy(queue *q);
int queue_empty(queue *q);  // New function declaration


#endif
