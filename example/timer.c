#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void timerHandler(int sig) {
    printf("SIGVTALRM raised\n");
    fflush(stdout);
}

int main() {

    // set the action associated with the SIGVTALRM interrupt
    struct sigaction timeraction;
    memset(&timeraction, 0, sizeof(timeraction));
    timeraction.sa_handler = &timerHandler;
    timeraction.sa_flags = 0;
    int ret = sigemptyset(&(timeraction.sa_mask));
    if(ret == -1){
        printf("Signal empty failed...\n");
        exit(-1);
    }
    ret = sigaddset(&(timeraction.sa_mask), SIGVTALRM);
    if(ret == -1){
        printf("Signal add failed...\n");
        exit(-1);
    }
    ret = sigaction(SIGVTALRM, &timeraction, NULL);
    if(ret == -1){
        printf("Setting signal action failed...\n");
        exit(-1);
    }

    // SIGVTALRM every 2 seconds
    // In you code, the quantum is given as microseconds, convert it to seconds if needed
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 250000;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 250000;

    // Set the timer
    ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);
    if(ret == -1) {
        printf("Timer failed...\n");
        exit(-1);
    }
    
    // Block SIGVTALRM temporarily
    ret = sigprocmask(SIG_BLOCK, &timeraction.sa_mask, NULL);
    if(ret == -1) {
        printf("Blocking failed...\n");
        exit(-1);
    }

    printf("SIGVTALRM blocked for 5 seconds...\n");
    sleep(5);  // Simulate some work

    // Unblock SIGVTALRM
    ret = sigprocmask(SIG_UNBLOCK, &timeraction.sa_mask, NULL);
    if(ret == -1) {
        printf("Unblocking failed...\n");
        exit(-1);
    }

    printf("SIGVTALRM unblocked\n");

    // Continue execution, allowing SIGVTALRM to be delivered
    while (1);

    return 0;
}