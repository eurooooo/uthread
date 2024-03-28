#include "uthread.h"

void function(void *arg) {

    int thdcounter = 1;
    int ret = 0;
    int id = uthread_self();
    for (int i = 0; i < 3; i++) {
        ret = uthread_yield();
        if(ret == -1){
            printf("Error in yield\n");
            exit(-1);
        }
        printf("ULT[%d](%d){%d}...YIELD[%d]\n", id, thdcounter++, uthread_priority(id), i);
    }

    printf("ULT[%d](%d){%d}...DONE...\n", id, thdcounter, uthread_priority(id));
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

    int priority = 1;
    int id1 = uthread_create(function, &priority, priority);
    if(id1 == -1){
        printf("Error in creating thread\n");
        exit(-1);
    }
    printf("MAIN(%d)...CREATE[%d]\n", mcounter++, id1);

    int ret = 0;
    for(int i = 0; i < 5; i++){
        ret = uthread_yield();
        if(ret == -1){
            printf("Error in yield\n");
            exit(-1);
        }
        printf("MAIN(%d)...YIELD[%d]\n", mcounter++, i);
    }
    
    printf("MAIN SHUTDOWN...\n");
    ret = uthread_join(id1);
    if(ret == -1){
        printf("Error in join\n");
        exit(-1);
    }
    printf("MAIN(%d) : [%d] JOINED\n", mcounter, id1);
    
    printf("COMPLETED SHUTDOWN...\n");
    
    return 0;
}