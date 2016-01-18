#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

Queue* setup_queue() {
    Queue* to_ret = (Queue*) malloc(sizeof(Queue));
    to_ret->head = NULL;
    to_ret->tail = NULL;
    to_ret->length = 0;
    return to_ret;
}

void enqueue(Queue* q, MyThread* thread) {
    if(queue_is_empty(q)) {
        q->head = thread;
        q->tail = thread;
    }
    else {
        q->tail->next = thread;
        q->tail = thread;
    }
    q->length++;   
}

int* dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        return NULL;
    }
    MyThread* thread = q->head;
    q->head = q->head->next;
    /* You do not need to handle 
     * freeing of the things here
     * because the thread engine's
     * respondibility will be to
     * remove the thread info 
     * when the thread is done. */
    return thread;
}

int queue_length(Queue* q) { 
    return q->length;
}

int queue_is_empty(Queue* q) {
    if(queue_length(q) == 0) {
        return TRUE;
    }
    return FALSE;
}
