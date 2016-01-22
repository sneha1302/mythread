#include <stdio.h>
#include <stdbool.h>
#include "mythread.h"
//#include "queue.h" /* THIS MUST BE INCLUDED AFTER mythread.h AND NOT BEFORE */
#include <ucontext.h>

unsigned int __get_next_available_tid();
/**************************************************************************************
 *************************************** MACROS ***************************************
 **************************************************************************************/

#define MAX_THREADS 65535 /* The MAX_INT of 32-bit systems just in case */
#define MAX_TID 4294967295
#define STACK_SIZE 8192
#define EXIT 0
#define RUN 1
#define READY 2
#define WCHLD 3
#define WALL 4




/**************************************************************************************
 ******************************** TYPEDEFS AND STRUCTS ********************************
 **************************************************************************************/
typedef struct __my_t {
    unsigned int tid;                   /* This thread's id */
    struct __my_t* parent;              /* Parent thread */
    int status;                         /* -### waiting on tid ### */
    __my_t child_list[MAX_THREADS];     /* list of children processes */
    unsigned int ct_cnt;                /* Child thread count */
    ucontext_t context;                 /* The context of this thread */
} __my_t



/**************************************************************************************
 ********************************** GLOBAL VARIABLES **********************************
 **************************************************************************************/
/* INVOKING_T
 * After MyThreadInit, we will need to keep track of which threads put us back in this library. To
 * do so, we will simply assign it the "current" tid whenever we yield. May need to modify this as
 * we go, but the idea is a way for us to keep track of parent ids when jumping vertically in the
 * hierarchy. -1 means the main thread.
 */
 __my_t * invoking_t

/* READY_Q
 * This queue will house threads that are simply waiting to run. When a thread has yielded, it gets
 * queued here. If a thread terminates, then nothing happens with the terminating thread. After
 * either, the head of this queue will be removed and ran. When I am empty, then the program has
 * finished via MyThreadExit().
 */
//Queue ready_q;

/* JOINING_Q
 * This queue will house any threads that are blocking on MyThreadJoin or MyThreadJoinAll
 * When either are called, the thread waiting_thread will be queued. When a thread exists, we will
 * need to check for its parent in here. If it exists in here, then we will need to remove the tid
 * of the child from ct_list and decrement ct_cnt of the thread. If and only if ct_cnt is 0, then
 * the thread can be removed from this queue and queued into the ready queue
 */
//Queue joining_q;

__my_t * main_t;

unsigned long avail_tids[MAX_TID];
unsigned long last_tid = 0;
__my_t threads[MAX_THREADS];
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

    /* Setting up and creating the context */
    char uctx_stack[STACK_SIZE];
    ucontext_t uctx;
    getcontext(&uctx);
    uctx.uc_stack.ss_sp = uctx_stack;
    uctx.uc_stack.ss_size = sizeof(uctx_stack);
    uctx.uc_link = main_t->context;
    makecontext(&uctx, start_funct, 1, args);

    /* Setting up and creating the thread */
    __my_t * t = (__my_t *) malloc(sizeof(__my_t));
    t->tid = __get_next_available_tid();
    t->parent = invoking_t;
    t->parent->child_list[t->parent->ct_cnt] = t;
    t->parent->ct_cnt = t->parent->ct_cnt + 1;
    t->status = READY;
    t->ct_cnt = 0;
    t->context = uctx;
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
  return 0
}


void MyThreadJoinAll(void){

    /* This will loop in a while loop yielding until it's ct_cnt is 0
    */
    return; //will change once implemented
}

/*
 * Exists the thread
 */
void MyThreadExit(void){
    //free(ready_q);
    //free(joining_q);
    //free(main_t);
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
    //ready_q = setup_queue();
    //joining_q = setup_queue();
    int i;
    for(i = 0; i < MAX_TID; i++) {
        avail_tids[i] = TRUE;
    }
    avail_tids[0] = FALSE;

    /* Setting up the main thread. It is unecessary to call makecontext() with
       the main thread as it is already in a function. */
    main_t->tid = 0;
    //main_t->ptid = -1;
    main_t->parent = NULL;
    main_t->active = 0;
    main_t->ct_cnt = 0;
    invoking_t = main_t;

    MyThreadCreate(start_funct, args);
    MyThreadJoinAll();
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
            avail_tids[i] == FALSE;
            last_tid = i;
            return i;
        }
    }
    return -1;
}
