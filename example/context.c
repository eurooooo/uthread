// Error checking is not added in the demo code.
// However, it is required in your solution code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

#define STACK_SZ 4096
#define uS_TO_S 1000000

ucontext_t context[2];

void switchThreads() {
    static int i = 0;
    volatile int flag = 0; // why is the variable volatile?
    int ret = getcontext(&context[i]);

    if(flag == 1) // handle double return, why is this required?
        return;
    
    flag = 1;
    i = (i + 1) % 2;
    setcontext(&context[i]);
}

void func1(void *arg){
    int id = *((int *)arg);
    int i = 0;
    while (1) {
        ++i;
        printf("%d -- %d\n", i, id);
        if (i % 5 == 0)
            switchThreads(); // context switch
        usleep(uS_TO_S);
    }
}

void initializeContext(void(*f)(void*), int i){
    int *id = (int *) malloc(sizeof(int));
    *id = i;
    
    getcontext(&context[i]); // get the current context
    // why is the double return not an issue here?

    context[i].uc_stack.ss_sp = (char *)malloc(sizeof(char) * STACK_SZ);
    context[i].uc_stack.ss_size = STACK_SZ;
    context[i].uc_stack.ss_flags = 0;

    makecontext(&context[i], (void (*)(void))f, 1, id);
}

int main(int argc, char *argv[]) {
    // initialize two context
    initializeContext(func1, 0);
    initializeContext(func1, 1);

    // set the first context to execute
    setcontext(&context[0]);

    // The following lines are never executed. In order to have a graceful return in your code
    // you should use a stub function as mentioned in the project description
    free(context[0].uc_stack.ss_sp); // never executed
    free(context[1].uc_stack.ss_sp);

    return 0;
}