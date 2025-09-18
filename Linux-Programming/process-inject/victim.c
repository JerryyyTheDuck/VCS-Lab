#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(){
    printf("Victim process started with PID: %d\n", getpid());
    printf("Waiting for injection...\n");
    
    while(true){
        printf("Victim process still running, PID: %d\n", getpid());
        sleep(5);  // Sleep for 5 seconds instead of 5000
    }
    return 0;
}