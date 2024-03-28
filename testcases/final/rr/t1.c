#include "uthread.h"

void function(void *arg) {

   for (int i = 0; i < 3; i++) {
      printf("ULT...[%d] - [%d]\n", uthread_self(), i);
   }

   printf("ULT...[%d] DONE...\n", uthread_self());
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

   int t1 = 1;
   int id1 = uthread_create(function, &t1);
   if(id1 == -1){
      printf("Error in creating thread\n");
      exit(-1);
   }
   printf("MAIN(%d)...CREATE[%d]\n", mcounter++, id1);
   
   printf("MAIN SHUTDOWN...\n");
   int ret = uthread_join(id1);
   if(ret == -1){
      printf("Error in join\n");
      exit(-1);
   }
   printf("MAIN(%d) : [%d] JOINED\n", mcounter++, id1);
   
   printf("COMPLETED SHUTDOWN...\n");
   
   uthread_exit();
   return 0;
}