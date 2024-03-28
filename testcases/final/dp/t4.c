#include "uthread.h"

void function(void *arg) {
    int val = *((int *)(arg));

    int thdcounter = 1;
    int id = uthread_self();
    for (int i = 1; i <= val; i++);

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

    int val[3] = {40000000, 2000000, 100000};
    int priority[3] = {1, 3, 2};
    int thds[3];
    for(int i = 0; i < 3; i++){
        thds[i] = uthread_create(function, &val[i], priority[i]);
        if(thds[i] == -1){
            printf("Error in creating thread\n");
            exit(-1);
        }
        printf("MAIN(%d)...CREATE[%d]\n", mcounter++, thds[i]);
    }

    printf("MAIN 1st SHUTDOWN...\n");
    int ret = 0;
    for(int i = 0; i < 3; i++){
        ret = uthread_join(thds[i]);
        if(ret == -1){
            printf("Error in join\n");
            exit(-1);
        }
        printf("MAIN(%d) : [%d] JOINED\n", mcounter++, thds[i]);
    }
    printf("COMPLETED 1st SHUTDOWN...\n");
    
    val[0] = 100000;
    val[1] = 2000000;
    val[2] = 40000000;
    priority[0] = 3;
    priority[1] = 2;
    priority[2] = 1;
    for(int i = 0; i < 3; i++){
        thds[i] = uthread_create(function, &val[i], priority[i]);
        if(thds[i] == -1){
            printf("Error in creating thread\n");
            exit(-1);
        }
        printf("MAIN(%d)...CREATE[%d]\n", mcounter++, thds[i]);
    }

    printf("MAIN 2nd SHUTDOWN...\n");
    for(int i = 0; i < 3; i++){
        ret = uthread_join(thds[i]);
        if(ret == -1){
            printf("Error in join\n");
            exit(-1);
        }
        printf("MAIN(%d) : [%d] JOINED\n", mcounter++, thds[i]);
    }
    printf("COMPLETED 2nd SHUTDOWN...\n");
    
    return 0;
}