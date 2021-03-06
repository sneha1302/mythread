#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

void f2();
void f1();
void setup_uctx();

#define STACK_SIZE (8 * 1024)
ucontext_t uctx_main;
ucontext_t uctx_func1;

void f1() {
    printf("Function 1 entered\n");
    printf("Function 1 switching to Function 1\n");

    swapcontext(&uctx_func1, &uctx_func1);

    printf("Function 1 returning\n");
}


int main(int argc, char* argv[]) {
    //char* func1_stack = (char*) malloc(STACK_SIZE);
    //char* func2_stack = (char*) malloc(STACK_SIZE);
    char func1_stack[STACK_SIZE];
    char func2_stack[STACK_SIZE];

    printf("Starting Main\n");
    getcontext(&uctx_func1);
    printf("Got context \n");
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    uctx_func1.uc_link = &uctx_main;
    makecontext(&uctx_func1, f1, 0);

    printf("Main swapping to Function 1\n");
    swapcontext(&uctx_main, &uctx_func1);
    printf("Main is exiting\n");

}
