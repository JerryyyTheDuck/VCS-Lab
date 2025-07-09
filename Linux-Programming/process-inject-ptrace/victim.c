#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() { 
    while (1) {
        printf("Current process is running %d\n", getpid());
        sleep(10);
    }

    return 0;
}