#include "mythread.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


typedef struct Node {
    __my_t* t;
    struct Node* next;
} Node;

typedef struct Queue {
    int length; 
    Node* head;
    Node* tail;
} Queue;

Queue* setup_queue();
void enqueue(Queue* q, __my_t* t);
int queue_is_empty(Queue* q);
int queue_length(Queue* q); 
void print_queue(Queue* q);
__my_t* dequeue(Queue* q);

