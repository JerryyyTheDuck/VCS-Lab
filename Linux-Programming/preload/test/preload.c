#define _GNU_SOURCE
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef int (*originalRand)(void);

int rand(){
    originalRand originRand = (originalRand)dlsym(RTLD_NEXT, "rand");
    return 1000;
}
