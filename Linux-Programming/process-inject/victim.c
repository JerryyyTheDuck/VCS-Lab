#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
int main(){
    while(true){
        printf("Current process id: %d\n", getpid());
        sleep(5000);
    }
    return 0;
}