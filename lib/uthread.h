/*
    Header: uthread.h
    Author: Austin Korpi, Zephyr Lin

    Instructions: 
        * Header file for uthread APIs
        * You may add extra data structures as required
        * Do not change the signature of ULT APIs
        * ustructs.h current hosts a few sample data structures you may use for the project
        * Feel free to use the ustructs.h header and associated source file for data structure manipulation
        * You may also use a uthread.h and uthread.c in your project for all the coding 
*/
#ifndef UTHREAD_H_
#define UTHREAD_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#define uS_TO_S 1000000 // microsec to sec
#define MAIN_THREAD 0   // Main thread ID

/******************************/
/*     Context Switch         */
/******************************/
/*
* Function:
*   You may define your own function for context switching, switchThreads is just a name we have given
* Arguments:
*   You may define it
* Returns:
*   You may define it
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Get the context of the current thread and store it in its TCB
    * Pick a thread from ready queue depending on the scheduler properties
    * Switch to the picked thread by setting it as the running thread
    * Set the current context to the picked threads context
*/
int switchThreads(void);

/******************************/
/*           Timer            */
/******************************/

/*
* Define functions to enable and disable time interrupts
* Define functions to set/reset timer depending on the situation
* Define timer handler function to trigger context switching every quanta
*/

int blockTimer();
int unblockTimer();
int startTimer();
int resetTimer();
void timerHandler(int sig);

/******************************/
/*         Scheduler          */
/******************************/

/*
* Decides the next ULT to switch to, returns its id
*/
int scheduler();

/************************************/
/*          ULT APIs                */
/*  DO NOT CHANGE ULT API SIGNATURE */
/************************************/

//////////////////////////////////////
/*
* Function:
*   uthread_init
* Arguments:
*   scheduler   : RR/SP/DP
*   quantum     : User input for quantum in microseconds
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Set the scheduler
    * Process the arguments according to scheduler
    * Set the timer interrupt
    * Initialize required queues
    * Initialize any scheduler specific values
    * Initialize main thread TCB and set it as running
    * Start timer 
*/
int uthread_init(char *scheduler, int quantum, ...);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_create
* Arguments:
*   start_routine   : The routine a ULT has to run
*   arg             : Arguments required by the routine
* Returns:
*   ULT id on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * Generate thread ID
    * Create ULT TCB and keep track
    * Make the context of TCB with require routine and arguments
    * Enqueue to READY
*/
int uthread_create(void(*start_routine), void *arg, ...);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_join
* Arguments:
*   tid   : ID of thread that the calling thread want to join
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * Check thread with tid has finished execution
    * If not, the current thread will have to wait till the tid thread is finished
    * You will have to use JOIN and FINISHED queues
    * Context switch may be required
*/
int uthread_join(int tid);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_yield
* Arguments:
*   NA
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * Change the state of running thread
    * Enqueue the running thread into one of the queues(?)
    * Context switch may be required
*/
int uthread_yield(void);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_exit
* Arguments:
*   NA
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * If there are any threads waiting for current thread to exit, take necessary steps
    * Enqueue current thread into one of the queues(?)
    * Context switch may be required
    * What happens if the main thread calls uthread_exit
*/
void uthread_exit(void);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_suspend
* Arguments:
*  tid  : Calling thread suspends thread with id tid
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * The thread tid's state is changed
    * Thread tid is enqueue into one of the queues(?)
    * Depending on whether the thread tid is running or waiting, you may have to take slight different steps
    * Context switch may be required
*/
int uthread_suspend(int tid);
//////////////////////////////////////

//////////////////////////////////////
/*
* Function:
*   uthread_resume
* Arguments:
*  tid  : Calling thread resumes thread with id tid
* Returns:
*   0 on success, -1 on failure
* Functionality: Abstract level details, there will be other steps in the logic to make it fully functional
    * Enable and disable interrupts at start and end if required
    * Dequeue thread tid from one of the queues(?)
    * The thread tid's state is changed
    * Thread tid is enqueue into one of the queues(?)
*/
int uthread_resume(int tid);
//////////////////////////////////////

//////////////////////////////////////

/*
* Function:
*   uthread_self
* Arguments:
*  NA
* Returns:
*   tid of running thread on success, -1 on failure
*/
int uthread_self(void);
//////////////////////////////////////

//////////////////////////////////////

/*
* Function:
*   uthread_priority
* Arguments:
*  tid: ID of thread whose priority should be retrieved
* Returns:
*   priority of thread tid on success, -1 on failure
*/
int uthread_priority(int tid);
//////////////////////////////////////

/*
* Functionality:
*   Run the start_routine with argument arg
*   Call uthread_exit
*   Enable and disable timers as required -> Think about this
*   You may move the stub function to other files
*/
void stub(void(*start_routine)(void *), void *arg);

/*
* Functionality:
*   Free all the resouces before main thread exit
*/
void uthread_free();

/*
* Functionality:
*   Used in the inter testcases
*   Print all the ready queues
*/
void print_ready_queue();

/*
* Functionality:
*   Free the resources of the thread with the ID 'tid'
*/
void free_thread(int tid);

/*
* Functionality:
*   For Dynamic Priority   
*   Change the priorities of running and waiting threads
*/
void adjust_priorities();

#endif