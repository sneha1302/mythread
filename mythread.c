#include <stdio.h>
//#include <stdbool.h>
//#include <ucontext.h>
#include <stdlib.h>
#include "mythread.h"
#include "mystuff.h" /* THIS MUST BE INCLUDED AFTER mythread.h AND NOT BEFORE */


/* TODO
 * Need to figure out remove references without dereferencing garbage
 * IE remove references of a child from parent and grandchild when parent
 * and gc are alive, when both are dead, and when either one but not the other is alive
 *
 */

/**************************************************************************************
 ********************************** GLOBAL VARIABLES **********************************
 **************************************************************************************/
/* current_T
 * After MyThreadInit, we will need to keep track of which threads put us back in this library. To
 * do so, we will simply assign it the "current" tid whenever we yield. May need to modify this as
 * we go, but the idea is a way for us to keep track of parent ids when jumping vertically in the
 * hierarchy. -1 means the main thread.
 */
__my_t* current_t;

/* runq
 * This queue will house threads that are simply waiting to run. When a thread has yielded, it gets
 * queued here. If a thread terminates, then nothing happens with the terminating thread. After
 * either, the head of this queue will be removed and ran. When I am empty, then the program has
 * finished via MyThreadExit().
 */
List* runq;

/* waitq
 * This queue will house any threads that are blocking on MyThreadJoin or MyThreadJoinAll
 * When either are called, the thread waiting_thread will be queued. When a thread exists, we will
 * need to check for its parent in here. If it exists in here, then we will need to remove the tid
 * of the child from ct_list and decrement ct_cnt of the thread. If and only if ct_cnt is 0, then
 * the thread can be removed from this queue and queued into the ready queue
 */
List* waitq;

__my_t* main_t;
__my_t* invoking_t;
unsigned int avail_tids[MAX_TID];
unsigned long last_tid = 0;
unsigned int t_cnt = 0;
char in_main;



/**************************************************************************************
 ************************************* FUNCTIONS **************************************
 **************************************************************************************/

/*
 * creates the thread and puts it into ready queue
 *
 */
MyThread MyThreadCreate (void(*start_funct)(void *), void *args) {

    /* 
     * Don't make another thread if we have too many! 
     */
    if(runq->length == MAX_THREADS) {
        return NULL;
    }
    int tid = __get_next_available_tid();
    if(tid == -1) {
        printf("No more available TID");
        return NULL;
    }
    /* 
     * Setting up and creating the context 
     */
    //char uctx_stack[STACK_SIZE];  DO NOT USE ARRAYS WITH CONTEXTS
    ucontext_t uctx;
    getcontext(&uctx);
    //uctx.uc_stack.ss_sp = uctx_stack;  DON'T DO IT. YOU WILL CRY BLOOD
    uctx.uc_stack.ss_sp = malloc(STACK_SIZE);
    //uctx.uc_stack.ss_size = sizeof(uctx_stack);
    uctx.uc_stack.ss_size = STACK_SIZE;
    uctx.uc_link = &(main_t->context);
    makecontext(&uctx, start_funct, 1, args);

    /* 
     * Setting up and creating the thread 
     */
    __my_t* t = (__my_t*) malloc(sizeof(__my_t));
    //t->tid = __get_next_available_tid();
    t->tid = tid;
    t->parent = current_t;
    /*
    int i;
    for(i = 0; i < MAX_THREADS; i++) {
        if(t->parent->child_list[i] == NULL) {
            t->parent->child_list[i] = t;
            break;
        }
    }*/
    t->child_list = setup_list();
    enqueue(t->parent->child_list, t);
    t->parent->ct_cnt = t->parent->ct_cnt + 1;
    t->status = READY;
    t->ct_cnt = 0;
    t->context = uctx;
    //clear_array(t);

    enqueue(runq, t);
    return (MyThread) t;
}

/*
 * Puts the thread back on the wait queue
 */
void MyThreadYield(void) {
    printf("DEBUG: Entered Yield\n");

    /*
     * Change the status and enqueue to runq
     */
    current_t->status = READY;
    enqueue(runq, current_t);
    invoking_t = current_t;
    printf("DEBUG: Yield: Queued the current thread %d\n", current_t->tid);

    /*
     * Grab the next thread to run and mark it as such
     */
    current_t = dequeue(runq); 
    if(current_t == NULL) {
        printf("DEBUG: Yield: FOUND NULL FROM RUNQ\n");
        current_t = main_t;
        exit(1); // SANITY CHECK ONLY we should never hit this
    }
    current_t->status = RUN;
    printf("DEBUG: Yield: SWAPPING!!! %d -> %d\n", invoking_t->tid, current_t->tid);
    swapcontext(&(invoking_t->context), &(current_t->context));
}




/*
 * puts thread on join queue and wait for the joining thread to complete
 */
int MyThreadJoin(MyThread thread){

    if(thread == NULL) {
        return -1;
    }
    /*
     * Need to first determine if the current thread even has thread as a child 
     */
    printf("DEBUG: Entered Join\n");
    __my_t* child_t = (__my_t*) thread;
    int found = __is_child(current_t, child_t);
    /*
    int i;
    int found = FALSE; 
    __my_t* child_t;
    for(i = 0; i < MAX_THREADS; i++) {
        child_t = current_t->child_list[i];
        if(child_t != NULL && requested_t->tid == child_t->tid) {
            found = TRUE;
            break;
        }
    }
    */
    if(!found) {
        printf("DEBUG: Join: Did not find child\n");
        return -1;
    }
    printf("DEBUG: Join: Found child\n");

    /*
     * Indicate which child we are now waiting on 
     */
    current_t->status = WCHLD;
    current_t->wc_tid = child_t->tid;
    enqueue(waitq, current_t);
    invoking_t = current_t;
    printf("DEBUG: Join: Queued the current thread to waitq %d\n to wait on %d\n", current_t->tid, child_t->tid);

    /*
     * Switch contexts with the next thread in the queue
     */
    current_t = dequeue(runq); 
    if(current_t == NULL) {
        printf("DEBUG: Join: FOUND NULL FROM RUNQ!\n");
        exit(1);
        // SANITY CHECK ONLY we should never hit this
    }
    printf("DEBUG: Join: SWAPPING!!! %d -> %d\n", invoking_t->tid, current_t->tid);
    swapcontext(&(invoking_t->context), &(current_t->context));
    return 0;
}




void MyThreadJoinAll(void){
    printf("DEBUG: entered JoinAll\n");

    /*
     * If we do not have children, then do not wait
     */
    if(current_t->ct_cnt == 0) {
        return;
    }

    /*
     * Modify the current thread to indicate waiting for all children to complete
     */
    current_t->status = WALL;
    enqueue(waitq, current_t);
    invoking_t = current_t;
    printf("DEBUG: Queued the current thread %d\n", current_t->tid);

    /*
     * If not in main, simply grab the next runnable thread swap
     */
    if(!in_main) {
        current_t = dequeue(runq);
        printf("DEBUG: JoinAll: SWAPPING!!! %d -> %d\n", invoking_t->tid, current_t->tid);
        swapcontext(&(invoking_t->context), &(current_t->context));
    }

    /*
     * Otherwise, we will need to spin in a loop waiting for all children to finish
     * cleaning them up when they do finish
     */
    else {
        while(!is_empty(runq)) {

            /*
             * Grab the next runnable thread 
             */
            current_t = dequeue(runq);
            if(current_t == NULL) {
                printf("DEBUG: JoinAll: FOUND NULL FROM RUNQ!\n");
                current_t = main_t;
                break; // SANITY CHECK ONLY we should never hit this
            }
            else if(current_t->tid == MAIN_TID && is_empty(runq)) {
                return;
            }

            /*
             * Indicate we are about to be in a userspace thread and swap
             */
            in_main = FALSE;
            printf("DEBUG: joinAll SWAPPING!!! %d -> %d\n", main_t->tid, current_t->tid);
            swapcontext(&(main_t->context), &(current_t->context));

            /*
             * If the thread exited, then clean it up
             */
            if(current_t->status == EXIT) {
                printf("DEBUG: Removing thread %d within thread %d\n", current_t->tid, main_t->tid);
                free(current_t->child_list);
                free(current_t);
                in_main = TRUE;
                current_t = main_t;
            }
        }
    }
}

/*
 * Exists the thread
 */
void MyThreadExit(void){
    printf("DEBUG: entered Exit \n");

    /*
     * Nothing to do if we are main
     */
    if(current_t->tid == MAIN_TID) {
        return;
    }

    /*
    __my_t* parent = current_t->parent;
    int i;
    if(parent != NULL) {
    */
        /* If the parent was waiting on us, requeue parent */
    /*
        if(parent->status == WCHLD && parent->wc_tid == current_t->tid ||
           parent->status == WALL && parent->ct_cnt == 0) {
               remove_from_queue(waitq, parent->tid);
               parent->status = READY;
               enqueue(runq, parent);
        }
    */
        /* Remove current from the parent's list of children */
    /*
        for(i = 0; i < parent->ct_cnt; i++) {
            __my_t* ct = parent->child_list[i];
            if(ct != NULL && ct->tid == current_t->tid) {
                parent->child_list[i] = NULL;
                parent->ct_cnt--;
                printf("DEBUG: Removed myself (%d) from parent (%d)\n", current_t->tid, parent->tid);
                break;
            }
        }
    }
    */

    __remove_refs(current_t);
    printf("DEBUG: Exit: Removed refs\n");
    /* Free the tid for use elsewhere */
    avail_tids[current_t->tid] = TRUE;

    /* if the parent no longer has children, and if it is WCHLD,
       then queue parent in ready */

    printf("DEBUG: SWAPPING!!! %d -> %d\n", current_t->tid, main_t->tid);
    current_t->status = EXIT;
    swapcontext(&(current_t->context), &(main_t->context));
}

/*
 * abstraction for synchronization. Creates a sem
 */
MySemaphore MySemaphoreInit(int initialValue){

}


void MySemaphoreSignal(MySemaphore sem){

}


void MySemaphoreWait(MySemaphore sem){

}
int MySemaphoreDestroy(MySemaphore sem){

}

// ****** CALLS ONLY FOR UNIX PROCESS ******
// Create and run the "main" thread

void MyThreadInit (void(*start_funct)(void *), void *args){
    /* "similar to invoking MyThreadCreate immediately followed by MyThreadJoinAll"
     * So literally just create the structures and then call MyThreadCreate on the function so that
     * a new thread is created. From there, call MyThreadJoinAll so that all its children terminate.
     * This will then allow it to finish.
     */

    /* PSEUDO CODE
     *
     * Initialize queues and thread
     * Call MyThreadCreate with the new thread
     * Call MyThreadJoinAll to wait for all its children thread to finish
     */

     /* Setting up queues and available tid list */
    //runq = setup_queue();
    //waitq = setup_queue();
    int i;
    for(i = 0; i < MAX_TID; i++) {
        avail_tids[i] = TRUE;
    }
    avail_tids[0] = FALSE;

    /* Setting up the main thread. It is unecessary to call makecontext() with
       the main thread as it is already in a function. */
    main_t = (__my_t*) malloc(sizeof(__my_t));
    main_t->tid = 0;
    //main_t->ptid = -1;
    main_t->parent = NULL;
    main_t->status = READY;
    main_t->child_list = (List*) malloc(sizeof(List));
    main_t->ct_cnt = 0;
    main_t->wc_tid = -1;
    //clear_array(main_t);
    current_t = main_t;

    runq = setup_queue();
    waitq = setup_queue();
    printf("DEBUG: Created main_t\n");
    in_main = TRUE;
    MyThreadCreate(start_funct, args);
    MyThreadJoinAll();
    printf("DEBUG: Goodbye! :D\n");

    free(runq);
    free(waitq);
    free(main_t);
}


/*

*/
unsigned int __get_next_available_tid() {
    int i;
    for(i = last_tid + 1; i != last_tid; i++) {
      if(i == MAX_TID) {
        i = 1;
      }
        if(avail_tids[i] == TRUE) {
            avail_tids[i] = FALSE;
            last_tid = i;
            return i;
        }
    }
    return -1;
}

void __remove_refs(__my_t* t) {
    __my_t* parent = t->parent;
    //if the parent is already gone, skip to the child work
    if(parent != NULL) {
        remove_from_list(parent->child_list, t);
        parent->ct_cnt--;
        printf("DEBUG: __remove_refs: removed child from parent\n");
        //if the parent was waiting on us, requeue to runq 
        if((parent->status == WALL && parent->ct_cnt == 0) ||
           (parent->status == WCHLD && parent->wc_tid == t->tid)) {
            remove_from_list(waitq, parent);
            parent->status = READY;
            parent->wc_tid = -1;
            enqueue(runq, parent);
        }
    }
    else {
        printf("DEBUG: __remove_refs: parent of %d was null\n", t->tid);
    }
    //Orphan the children
    __my_t* iterator;
    while(!is_empty(t->child_list)) {
        iterator = dequeue(t->child_list);
        iterator->parent = NULL;
    }
}

int __is_child(__my_t* parent, __my_t* child) {
    Node* iterator = parent->child_list->head;
    while(iterator != NULL) {
        if(iterator->t->tid == child->tid) {
            return TRUE;
        }
        iterator = iterator->next;
    }
    return FALSE;
}
