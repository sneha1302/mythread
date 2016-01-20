#include <stdio.h>
#include <strings.h>
#include "mythread.h"
#include "queue.h"
#include <ucontext.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MAX_THREADS
#define MAX_THREADS 65535 /* The MAX_INT of 32-bit systems just in case */
#endif
#ifndef STACK_SIZE 8192
#endif

typedef void * _my_thread {
    //unsigned int tid;                  /* This thread's id */
    //unsigned int ptid;                 /* Parent thread id */
    struct _my_thread* parent;
    int active;                        /* whether or not it is running or in the wait queue */
    //unsigned int ct_list[MAX_THREADS]; /* Child thread id list. */
    _my_thread child_list[MAX_THREADS];
    unsigned int ct_cnt                /* Child thread count */
} _my_thread

int yielding_tid;       /* tid of the thread that invoked MyThread{Create,Yield,Join,JoinAll}  */

/* READY QUEUE
 * This queue will house threads that are simply waiting to run. When a thread has yielded, it gets
 * queued here. If a thread terminates, then nothing happens with the terminating thread. After 
 * either, the head of this queue will be removed and ran. When I am empty, then the program has
 * finished via MyThreadExit(). 
 */


/* JOIN QUEUE
 * This queue will house any threads that are blocking on MyThreadJoin or MyThreadJoinAll
 * When either are called, the thread waiting_thread will be queued. When a thread exists, we will 
 * need to check for its parent in here. If it exists in here, then we will need to remove the tid
 * of the child from ct_list and decrement ct_cnt of the thread. If and only if ct_cnt is 0, then 
 * the thread can be removed from this queue and queued into the ready queue
 */

/* YIELDING_TID
 * After MyThreadInit, we will need to keep track of which threads put us back in this library. To 
 * do so, we will simply assign it the "current" tid whenever we yield. May need to modify this as
 * we go, but the idea is a way for us to keep track of parent ids when jumping vertically in the
 * hierarchy
 */


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

}


void MyThreadJoinAll(void){

    /* This will loop in a while loop yielding until it's ct_cnt is 0
    */
}

/*
 * Exists the thread
 */
void MyThreadExit(void){

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
}
