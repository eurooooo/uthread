#include "uthread.h"

void function(void *arg) {
    int thdcounter = 1;
    int ret = 0;
    int id = uthread_self();
    for (int i = 0; i < 3; i++) {
        printf("ULT[%d](%d){%d}...YIELD[%d]\n", id, thdcounter++, uthread_priority(id), i);
        ret = uthread_yield();
        if(ret == -1){
            printf("Error in yield\n");
            exit(-1);
        }
    }

    printf("ULT[%d](%d){%d}...DONE...\n", id, thdcounter, uthread_priority(id));
}

int main(int argc, char *argv[]){
    char *scheduler = argv[1];
    int quantum = atoi(argv[2]);
    int x = atoi(argv[3]);

    if(uthread_init(scheduler, quantum, x) != 0){
        printf("Error in ULT Library initialization\n");
        exit(-1);
    }

    int mcounter = 1;
    printf("MAIN(%d)...[%d]\n", mcounter++, MAIN_THREAD);

    int priority[5] = {2, 1, 4, 3, 5};
    int thds[5];

    for(int i = 0; i < 5; i++){
        thds[i] = uthread_create(function, &priority[i], priority[i]);
        if(thds[i] == -1){
            printf("Error in creating thread\n");
            exit(-1);
        }
        printf("MAIN(%d)...CREATE[%d](%d)\n", mcounter++, i + 1, priority[i]);
    }
    // thread 2 has joined on thread 3
    int ret = 0;
    ret = uthread_suspend(thds[2]);
    if(ret == -1){
        printf("Error in suspend\n");
        exit(-1);
    }
    printf("MAIN(%d)...SUSPEND[%d]\n", mcounter++, thds[2]);
    // thread 3 exits (unblock thread 2)
    for(int i = 0; i < 5; i++){
        ret = uthread_yield();
        if(ret == -1){
            printf("Error in yield\n");
            exit(-1);
        }
        printf("MAIN(%d)...YIELD[%d]\n", mcounter++, i);
    }

    ret = uthread_resume(thds[2]);
    if(ret == -1){
        printf("Error in resume\n");
        exit(-1);
    }
    printf("MAIN(%d)...RESUME[%d]\n", mcounter++, thds[2]);
    
    printf("MAIN SHUTDOWN...\n");
    for(int i = 0; i < 5; i++){
        ret = uthread_join(thds[i]);
        if(ret == -1){
            printf("Error in join\n");
            exit(-1);
        }
        printf("MAIN(%d) : [%d] JOINED\n", mcounter, thds[i]);
    }
    
    printf("COMPLETED SHUTDOWN...\n");
    
    return 0;
}