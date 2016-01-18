#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

char stack[16384];

ucontext_t uctx ;

int main() {
    getcontext(&uctx);
    printf("Herpa\n");
    uctx.uc_stack.ss_sp = stack;
    return 0;
}
