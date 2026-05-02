#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int mypid = getpid();
    int ret, pid_zombie, pid_dead;

    printf("\n=== Testing co_yield edge conditions ===\n");

    // Test 1: Yield to a non-existent PID
    // Expectation: co_yield should return -1 because PID 9999 is not in use.
    ret = co_yield(9999, 1);
    printf("Test 1 (Invalid PID): %s\n", ret == -1 ? "PASS" : "FAIL");

    // Test 2: Yield to itself
    // Expectation: co_yield should return -1 to prevent a process from yielding to itself.
    ret = co_yield(mypid, 1);
    printf("Test 2 (Self-yield): %s\n", ret == -1 ? "PASS" : "FAIL");

    // Test 3: Yield to a killed process (Zombie / Dying state)
    // Expectation: co_yield should fail (-1) because the target process was killed 
    // but not yet reaped by wait(), meaning it exists in the table but cannot run.
    pid_zombie = fork();
    if (pid_zombie == 0) {
        for(;;) sleep(100); // Child waits to be killed
    }
    sleep(1);          // Ensure child is scheduled and running
    kill(pid_zombie);  // Mark child as KILLED (becomes zombie)
    ret = co_yield(pid_zombie, 1);
    printf("Test 3 (Yield to zombie process): %s\n", ret == -1 ? "PASS" : "FAIL");
    wait(0);           // Reap the zombie process to clean up the process table

    // Test 4: Yield to a completely reaped (dead) process
    // Expectation: co_yield should fail (-1) because the process has exited 
    // and wait() has completely removed it from the system.
    pid_dead = fork();
    if (pid_dead == 0) {
        exit(0);       // Child exits immediately
    }
    wait(0);           // Parent reaps the child immediately
    sleep(1);          // Give kernel time to finalize cleanup
    ret = co_yield(pid_dead, 1);
    printf("Test 4 (Yield to reaped process): %s\n", ret == -1 ? "PASS" : "FAIL");

    printf("\n=== Testing normal co_yield ===\n");
    
    // Test 5: Normal operation (Infinite ping-pong loop)

    int pid1 = getpid();
    int pid2 = fork();

    if(pid2 < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid2 == 0){
        for(;;){
            int value = co_yield(pid1, 1);
            printf("Child received: %d\n", value);
        }
    } else {
        for(;;){
            int value = co_yield(pid2, 2);
            printf("parent received: %d\n", value);
        }
    }
}
