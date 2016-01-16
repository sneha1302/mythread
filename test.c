#include <stdio.h>
#include <stdlib.h>


int main() {
    int a = 10;
    int* i;
    i = &a;
    int x = *i;

    printf("%d\n", x);
    free(i);
    printf("%d\n", x);
    return 0;
}
