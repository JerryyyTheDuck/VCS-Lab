#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>

typedef ssize_t (*originalRead)(int fd, void *buf, size_t count);

ssize_t read(int fd, void *buf, size_t count) {
    originalRead originRead = (originalRead)dlsym(RTLD_NEXT, "read");
    if (originRead == NULL) {
        fprintf(stderr, "Error: %s\n", dlerror());
    }
    printf("Preload hooked by nguyendh\n");
    return originRead(fd, buf, count);
}