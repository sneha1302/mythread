#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/*
int main() {

    __my_t* t1 = (__my_t*) malloc(sizeof(__my_t));
    t1->tid = 100;
    __my_t* t2 = (__my_t*) malloc(sizeof(__my_t));
    t2->tid = 200;
    Queue* queue = setup_queue();
    printf("%d\n", queue->length);
    enqueue(queue, t1);
    printf("%d\n", queue->length);
    enqueue(queue, t2);
    printf("%d\n", queue->length);
    __my_t* a = dequeue(queue);
    printf("%d\n", queue->length);
    __my_t* b = dequeue(queue);
    printf("%d\n", queue->length);
    printf("%d\n", a->tid);
    printf("%d\n", b->tid);


    print_queue(queue);
    //free(queue);
    int i;
    for(i = 0; i < 10; i++) {
        t1->tid = t1->tid + (10 * i);
        t2->tid = t2->tid + (10 * i);
        printf("i: %d\n", i);
        enqueue(queue, t1);
        enqueue(queue, t2);
        a = dequeue(queue);
        b = dequeue(queue);
        print_queue(queue);
        printf("%d\n", a->tid);
        printf("%d\n", b->tid);
    }

    return 0;
}
*/

Queue* setup_queue() {
    Queue* q = (Queue *) malloc(sizeof(Queue));
    q->length = 0;
    q->head = 0;
    q->tail = 0;
    return q;
}

void enqueue(Queue* q, __my_t* t) {
    if(queue_is_empty(q)) {
        q->t_list[q->tail] = t;
    }
    else {
        q->tail = (q->tail + 1) % MAX_THREADS;
        q->t_list[q->tail] = t;
    }
    q->length++;
}

__my_t* dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        return NULL;
    }
    __my_t * t = q->t_list[q->head];

    if(q->head != q->tail) {
        q->head = (q->head + 1) % MAX_THREADS;
    }
    q->length--;
    return t;
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

void print_queue(Queue* q) {
    printf("Head: %d\n", q->head);
    printf("Tail: %d\n", q->tail);
    printf("Length: %d\n", q->length);
}
