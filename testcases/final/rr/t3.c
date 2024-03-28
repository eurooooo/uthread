#include "uthread.h"

void function(void *arg) {

    int thdcounter = 1;
    int ret = 0;
    for (int i = 0; i < 3; i++) {
        ret = uthread_yield();
        if(ret == -1){
            printf("Error in yield\n");
            exit(-1);
        }
        printf("ULT[%d](%d)...YIELD[%d]\n", uthread_self(), thdcounter++, i);
    }

    printf("ULT[%d](%d)...DONE...\n", uthread_self(), thdcounter);
}

int main(int argc, char *argv[]){
    char *scheduler = argv[1];
    int quantum = atoi(argv[2]);

    if(uthread_init(scheduler, quantum) != 0){
        printf("Error in ULT Library initialization\n");
        exit(-1);
    }

    int mcounter = 1;
    printf("MAIN(%d)...[%d]\n", mcounter++, MAIN_THREAD);

    int val[5] = {1, 2, 3, 4, 5};
    int thds[5];

    for(int i = 0; i < 5; i++){
        thds[i] = uthread_create(function, &val[i]);
        if(thds[i] == -1){
            printf("Error in creating thread\n");
            exit(-1);
        }
        printf("MAIN(%d)...CREATE[%d]\n", mcounter++, i);
    }

    int ret = 0;
    ret = uthread_suspend(thds[2]);
    if(ret == -1){
        printf("Error in suspend\n");
        exit(-1);
    }
    printf("MAIN(%d)...SUSPEND[%d]\n", mcounter++, thds[2]);

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