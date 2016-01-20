#include <stdio.h>
#include <stdlib.h>


/*int main() {

    Queue* queue = setup_queue();
    printf("%d\n", queue->length);
    enqueue(queue, 1);
    printf("%d\n", queue->length);
    enqueue(queue, 2);
    printf("%d\n", queue->length);
    int* a = dequeue(queue);
    int* b = dequeue(queue);
    printf("%d\n", *a);
    printf("%d\n", *b);
    
    
    print_queue(queue);
    free(queue);

    return 0;
}*/

Queue* setup_queue() {
    Queue* to_ret = (Queue*) malloc(sizeof(Queue));
    to_ret->head = NULL;
    to_ret->tail = NULL;
    to_ret->length = 0;
    return to_ret;
}

void enqueue(Queue* q, __my_t* t) {
    Node* temp = (Node*) malloc(sizeof(Node));
    temp->t = t;
    temp->next = NULL;

    if(queue_is_empty(q)) {
        q->head = temp;
        q->tail = temp;
    }
    else {
        q->tail->next = temp;
        q->tail = temp;
    }
    q->length++;
        
}

__my_t* dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        return NULL;
    }
    /*  */
    //__my_t* t = q->head.t;
    __my_t* t = (__my_t) malloc(sizeof(__my_t));
    memcpy(t, q->head.t, sizeof(__my_t));
    Node* next = q->head->next;
    free(q->head);
    q->head = next;
    q->length = q->length - 1;
   
    /* We will need to free the node itself but not the thread.
     * The node itself has a bit of extra memory used */ 
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
    Node* temp = q->head;
    while(temp != NULL) {
        printf("%d\n", temp->data);
        temp = temp->next;
    }
}
