#include "mythread.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct __my_t {
    unsigned int tid;
} __my_t;
#define MAX_THREADS 65535

typedef struct Queue {
    __my_t * t_list[MAX_THREADS];
    unsigned int length;
    unsigned int head;
    unsigned int tail;
} Queue;

Queue* setup_queue();
void enqueue(Queue* q, __my_t* t);
int queue_is_empty(Queue* q);
int queue_length(Queue* q);
void print_queue(Queue* q);
__my_t* dequeue(Queue* q);
__my_t* remove_from_queue(Queue* q, unsigned int tid);
