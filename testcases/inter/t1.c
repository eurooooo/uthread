#include "uthread.h"

void function(void *arg) {
   // pass
}

int main(int argc, char *argv[]){

   char *scheduler = argv[1];
   int quantum = atoi(argv[2]);

   if(uthread_init(scheduler, quantum) != 0){
      printf("Error in ULT Library initialization\n");
      exit(-1);
   }

   int arg = 1;
   int id1 = uthread_create(function, &arg);
   if(id1 == -1){
      printf("Error in creating thread\n");
      exit(-1);
   }
   printf("MAIN...[%d]\n", MAIN_THREAD);

   // add a utility function to print the ready queue or print directly
   print_ready_queue();
   // print ULT...tid

   return 0;
}
