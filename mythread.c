#include <stdio.h>
#include <strings.h>
#include "mythread.h"
#include "queue.h"
#include <ucontext.h>

int main(int* args) {

    return 0;
}

/*
 * creates the thread and puts it into ready queue
 *
 */
MyThread MyThreadCreate (void(*start_funct)(void *), void *args){

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
void MyThreadInit (void(*start_funct)(void *), void *args){

}
