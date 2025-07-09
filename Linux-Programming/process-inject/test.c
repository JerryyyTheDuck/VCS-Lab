// The roadmap of the injector tool is simple:
// Compute the address of __libc_dlopen_mode into the victim process.
// Attach the victim process with ptrace.
// Modify the victim process to make it execute the __libc_dlopen_mode function

//https://dee-dee.fr/2020/04/10/shared-library-injection-into-a-linux-process-part-1/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <libgen.h>

unsigned long findStartAddress(int pid) {

    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("[-] Failed to open /proc/<pid>/maps");
        return 0; 
    }

    char libc_path[256];
    char line[256];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "r-xp") && strstr(line, "libc.so")) {
            char *start = strtok(line, "-");
            if (start) {
                unsigned long start_addr = strtoul(start, NULL, 16);
                return start_addr;
            }
            break;
        }
    }
    fclose(fp);
    return 0;
}

unsigned long returnFunctionAddress(const char *libc_name, const char *function_name) {
    printf("[+] Looking for function %s in library %s\n", function_name, libc_name);
    void *handle = dlopen(libc_name, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "[-] Failed to open library: %s\n", dlerror());
        return 0;
    }

    unsigned long func_addr = (unsigned long)dlsym(handle, function_name);
    if (!func_addr) {
        fprintf(stderr, "[-] Failed to find function %s: %s\n", function_name, dlerror());
        dlclose(handle);
        return 0;
    }

    dlclose(handle);
    return func_addr;
}

char* returnLibCName(int pid){
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("[-] Failed to open libc");
        return NULL;
    }
    char line[256];
    char *library_name = malloc(256);
    if (library_name == NULL) {
        perror("[-] Memory allocation failed");
        fclose(fp);
        return NULL;
    }


    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "r-xp") && strstr(line, "libc.so")) {
            char *libc_name = strrchr(line, ' ');
            if (libc_name) {
                strncpy(library_name, libc_name + 1, 255);
                library_name[255] = '\0';
                return library_name;
            }
        }
    }
    free(library_name);
    fclose(fp);
    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);
    printf("[+] Process ID to be injected: %d\n", pid);

    int current_pid = getpid();
    printf("[+] Current Process ID: %d\n", current_pid);

    unsigned long start_address = findStartAddress(pid);
    unsigned long current_start_address = findStartAddress(current_pid);

    printf("[+] Start address of libc in target process: 0x%lx\n", start_address);
    printf("[+] Start address of libc in current process: 0x%lx\n", current_start_address);

    char* orginal_libc_name = returnLibCName(current_pid);
    char* target_libc_name = returnLibCName(pid);
    if(strcmp(orginal_libc_name, target_libc_name) != 0) {
        printf("[-] The target process is not using the same libc as the current process.\n");
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }


    unsigned long function_address = returnFunctionAddress(target_libc_name, "dlopen");
    if (function_address == 0) {
        printf("[-] Failed to find __libc_dlopen_mode function address.\n");
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }
    printf("[+] Address of __libc_dlopen_mode in target process: 0x%lx\n", function_address);

    ulong offset = function_address - start_address;
    
    printf("[+] Length of offset%ld");




    return 0;
}