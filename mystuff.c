#include <stdio.h>
#include <stdlib.h>
#include "mystuff.h"

int len(List* l) {
    return l->length;
}

List* setup_list() {
    List* to_ret = (List*) malloc(sizeof(List));
    to_ret->head = NULL;
    to_ret->tail = NULL;
    to_ret->length = 0;
    return to_ret;
}

List* setup_queue() {
    return setup_list();
}

void enqueue(List* l, __my_t* t) {
    Node* n = (Node*) malloc(sizeof(Node));
    n->t = t;
    n->next = NULL;

    if(is_empty(l)) {
        n->prev = NULL;
        l->head = n;
        l->tail = n;
    }
    else {
        n->prev = l->tail;
        l->tail->next = n;
        l->tail = n;
    }
    l->length++;
}

__my_t* dequeue(List* l) {
    if(is_empty(l)) {
        return NULL;
    }
    Node* to_free = l->head;
    __my_t* t = to_free->t;
    if(l->length == 1) {
        l->head = NULL;
        l->tail = NULL;
    }
    else {
        l->head = l->head->next;
    }
    free(to_free);
    l->length--;
    return t;
}

void remove_from_list(List* l, __my_t* t) {
    unsigned int tid = t->tid;
    //If the queue is empty, return
    if(is_empty(l)) {
        return;
    }
    Node* iterator = l->head;
    Node* to_free;
    //If the list has one item and we found the item to remove:
    if(iterator->t->tid == tid) {
        dequeue(l);
        return;
    }
    //if the list has 2 or more items:
    while(iterator != NULL && iterator->next != NULL) {
        if(iterator->next->t->tid == tid) {
            to_free = iterator->next;
            iterator->next = to_free->next;
            //If we are somewhere in the middle of the list
            if(to_free->next != NULL) {
                to_free->next->prev = iterator;
            }
            to_free->next = NULL;
            to_free->prev = NULL;
            to_free->t = NULL;
            free(to_free);
            l->length--;
            return;
        }
        //continue searching for the next one
        iterator = iterator->next;
    }
        
}

int is_empty(List* l) {
    if(l->length == 0) {
        return TRUE;
    }
    return FALSE;
}

__my_t* peek(List* l) {
    if(is_empty(l)) {
        return NULL;
    }
    return l->head->t;
}

/*
int main() {
    List* l = setup_list();
    Node* iterator;
    __my_t* it;
    __my_t* a = (__my_t*) malloc(sizeof(__my_t));
    __my_t* b = (__my_t*) malloc(sizeof(__my_t));
    __my_t* c = (__my_t*) malloc(sizeof(__my_t));
    __my_t* d = (__my_t*) malloc(sizeof(__my_t));
    int w;
    int x;
    int y;
    int z;

    a->tid = 1;
    b->tid = 2;
    c->tid = 3;
    d->tid = 4;

    enqueue(l, a);
    if(is_empty(l)) {
        printf("Why is it empty?\n");
    }
    else {
        printf("Length: %d\n", l->length);
    }
    printf("Head: %d, Tail: %d, Length: %d\n", l->head->t->tid, l->tail->t->tid, l->length);
    enqueue(l, b);
    printf("Head: %d, Tail: %d, Length: %d\n", l->head->t->tid, l->tail->t->tid, l->length);
    enqueue(l, c);
    printf("Head: %d, Tail: %d, Length: %d\n", l->head->t->tid, l->tail->t->tid, l->length);
    enqueue(l, d);
    printf("Head: %d, Tail: %d, Length: %d\n", l->head->t->tid, l->tail->t->tid, l->length);
    iterator = l->head;
    while(iterator != NULL) {
        printf("Node: %d\n", iterator->t->tid);
        iterator = iterator->next;
    }
    while(!is_empty(l)) {
        it = dequeue(l);
        if(is_empty(l)) 
            printf("tid: %d, Head: %d, Tail: %d, Length: %d\n", it->tid, l->head, l->tail, l->length);
        
        else 
            printf("tid: %d, Head: %d, Tail: %d, Length: %d\n", it->tid, l->head->t->tid, l->tail->t->tid, l->length);
    }
    __my_t* null = dequeue(l);
    if(null == NULL) {
        printf("Null\n");
    }

    return 0;
}
*/
