#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    printf("Initial memory usage: %d bytes\n", memsize());
    
    // Allocate 20k bytes (20,000 bytes)
    void *ptr = malloc(20000);
    if (ptr == 0) {
        printf("Error: malloc failed\n");
        exit(1);
    }
    
    printf("Memory usage after malloc: %d bytes\n", memsize());
    
    // Free the allocated array
    free(ptr);
    
    printf("Memory usage after free: %d bytes\n", memsize());
    
    exit(0);
}