#include <stdio.h>
//#include <stdbool.h>
//#include <ucontext.h>
#include <stdlib.h>
#include "mythread.h"
#include "mystuff.h" /* THIS MUST BE INCLUDED AFTER mythread.h AND NOT BEFORE */



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
 Queue* runq;

/* waitq
 * This queue will house any threads that are blocking on MyThreadJoin or MyThreadJoinAll
 * When either are called, the thread waiting_thread will be queued. When a thread exists, we will
 * need to check for its parent in here. If it exists in here, then we will need to remove the tid
 * of the child from ct_list and decrement ct_cnt of the thread. If and only if ct_cnt is 0, then
 * the thread can be removed from this queue and queued into the ready queue
 */
Queue* waitq;

__my_t* main_t;
__my_t* invoking_t;
unsigned long avail_tids[MAX_TID];
unsigned long last_tid = 0;
unsigned int t_cnt = 0;

/**************************************************************************************
 ************************************* FUNCTIONS **************************************
 **************************************************************************************/
/*
 * creates the thread and puts it into ready queue
 *
 */
MyThread MyThreadCreate (void(*start_funct)(void *), void *args){
/*
 * So in order to make a new thread, we will need to create a new context. Assign shit about it,
 * and then enqueue it to the ready queue
 *
 * PSEUDO CODE
 * - Init the context in some uctx
 *   - getcontext()
 *   - assign stack stuff and link back to main
 * - Assign tid and ptid active ct_cnt ct_list OR the parent pointer and child list
 */

    /* Don't make another thread if we have too many! */
    if(runq->length == MAX_THREADS) {
        return NULL;
    }

    /* Setting up and creating the context */
    char uctx_stack[STACK_SIZE];
    ucontext_t uctx;
    printf("DEBUG: HERE I AM\n");
    getcontext(&uctx);
    uctx.uc_stack.ss_sp = uctx_stack;
    uctx.uc_stack.ss_size = sizeof(uctx_stack);
    uctx.uc_link = &(main_t->context);
    makecontext(&uctx, start_funct, 1, args);

    printf("DEBUG: Created context\n");
    /* Setting up and creating the thread */

    __my_t* t = (__my_t*) malloc(sizeof(__my_t));
    t->tid = __get_next_available_tid();
    t->parent = current_t;
    t->parent->child_list[t->parent->ct_cnt] = t;
    t->parent->ct_cnt = t->parent->ct_cnt + 1;
    t->status = READY;
    t->ct_cnt = 0;
    t->context = uctx;

    printf("DEBUG: Created thread\n");
    enqueue(runq, t);
    printf("DEBUG: Queued thread\n");
    return (MyThread) t;
}

/*
 * Puts the thread back on the wait queue
 */
void MyThreadYield(void) {

}




/*
 * puts thread on join queue and wait for the joining thread to complete
 */
int MyThreadJoin(MyThread thread){
  return 0;
}




void MyThreadJoinAll(void){
    printf("DEBUG: entered JoinAll\n");
    current_t->status = WALL;
    enqueue(waitq, current_t);
    invoking_t = current_t;
    printf("DEBUG: Queued the current thread %d\n", current_t->tid);
    while(!queue_is_empty(runq)) {
        current_t = dequeue(runq);
        if(current_t == NULL) {
            current_t = main_t;
            break; // SANITY CHECK ONLY we should never hit this
        }
        printf("DEBUG: SWAPPING!!! %d -> %d\n", invoking_t->tid, current_t->tid);
        swapcontext(&(invoking_t->context), &(current_t->context));
        if(current_t->status == EXIT) {
            free(current_t);
        }
    }

    /* Clean up! */
    return; //will change once implemented
}

/*
 * Exists the thread
 */
void MyThreadExit(void){
    /* Terminates the invoking thread. Note: all MyThreads are required to invoke
     * this function. Do not allow functions to “fall out” of the start function
     *
     * PSEUDO CODE
     * - Remove us from the parent list of child threads and decrement ct_cnt
     * - Free the tid for use elsewhere
     * - Check if the parent still exists. If not, then just leave
     * - Else check if parent is in WCHLD, moving it to ready if
     * - Free?
     */
    printf("DEBUG: entered Exit \n");
    if(current_t->tid == MAIN_TID) {
        return;
    }


    __my_t* parent = current_t->parent;
    /* Remove itself from the parent's list of children */
    int i;
    for(i = 0; i < parent->ct_cnt; i++) {
        __my_t* ct = parent->child_list[i];
        if(ct != NULL && ct->tid == current_t->tid) {
            parent->child_list[i] = NULL;
            parent->ct_cnt--;
            break;
        }
    }
    printf("DEBUG: Removed myself from parent \n");
    /* Free the tid for use elsewhere */
    avail_tids[current_t->tid] = TRUE;

    /* if the parent no longer has children, and if it is WCHLD,
       then queue parent in ready */

    if(parent->status == WCHLD && parent->wc_tid == current_t->tid ||
       parent->status == WALL && parent->ct_cnt == 0) {
           remove_from_queue(waitq, parent->tid);
           parent->status = READY;
           enqueue(runq, parent);
    }
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
    main_t->ct_cnt = 0;
    main_t->wc_tid = -1;
    current_t = main_t;

    runq = setup_queue();
    waitq = setup_queue();
    printf("DEBUG: Created main_t\n");
    MyThreadCreate(start_funct, args);
    MyThreadJoinAll();

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
