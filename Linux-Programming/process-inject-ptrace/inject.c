#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void __attribute__((constructor)) test(void) {
    printf("Hello world from injected library!\n");
    fflush(stdout);
}