#include <stdio.h>
#include <strings.h>
#include "mythread.h"

void f1(void *a) {
    printf("DEBUG: entered a \n");
}

int main() {
    MyThreadInit(f1, NULL);
}
