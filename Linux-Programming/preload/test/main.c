#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int random_number = rand();
    printf("Random number: %d\n", random_number);
    return 0;
}