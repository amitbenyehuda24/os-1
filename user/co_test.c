#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid1 = getpid(); // Parent PID
    int pid2 = fork();   // Child PID

    if (pid2 == 0) { 
        // Child process
        for (;;) {
            int value = co_yield(pid1, 1);
            printf("Child received: %d\n", value); // Should print 2
            exit(0); // Added to break the infinite loop for testing
        }
    } else { 
        // Parent process
        for (;;) {
            int value = co_yield(pid2, 2);
            printf("parent received: %d\n", value); // Should print 1
            
            // --- Error Handling Tests ---
            printf("Testing invalid PID: %d\n", co_yield(-5, 10)); // Expect -1
            printf("Testing self-yield: %d\n", co_yield(pid1, 10)); // Expect -1
            
            wait(0); // Wait for child to finish
            
            printf("Testing killed process: %d\n", co_yield(pid2, 10)); // Expect -1
            exit(0);
        }
    }
    return 0;
}