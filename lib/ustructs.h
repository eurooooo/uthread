/*
    Header: ustructs.h
    Author: Austin Korpi, Zephyr Lin

    Instructions: 
        * Header file for ULT metadata structures
        * You may add extra data structures as required
        * extern variables are declared in the file, you may move it across files as required
*/
#ifndef QUEUE_H_
#define QUEUE_H_

#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ucontext.h>

/*****************************/
#define MAX_THREADS 100
#define STACK_SZ 4096
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

typedef enum {
    RR, // Round robin
    SP, // Static priority
    DP  // Dynamic priority
} schedulerType;

extern schedulerType stype;
/******************************/
/*           TCB              */
/******************************/
typedef enum { // You may add other thread states according to your programming logic
    READY,
    RUNNING,
    BLOCKED,
    JOINED,
    FINISHED
} threadState;

typedef struct {
    int id_;                // thread id
    threadState state_;     // thread state
    ucontext_t context_;    // thread context (stores registers, pc)
    char *stack_;           // thread stack
    int priority_;          // thread priority
    int waiting;
    int timeWaiting;
} TCB;

extern TCB *t_running; // keeps track of running thread
extern TCB *threads[MAX_THREADS];   // array placeholder for TCBs; threads[0] is always the main thread


/******************************/
/*           Queue            */
/******************************/
/*
You may define your own queue structure and use any type of queue that will suit your ULT design
Properly document the queue structure and associated functions in the header or source file
Queue properties for RR, SP and DP schedulers will differ 
*/

typedef struct QueueNode{
    struct QueueNode *next;
    struct QueueNode *prev;
    TCB *thread;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* back;
    int size;
} ThreadQueue;

void threadQueue_init(ThreadQueue *);
int threadQueue_enque(ThreadQueue *, TCB *);
TCB* threadQueue_deque(ThreadQueue *);
void threadQueue_delete(ThreadQueue *, TCB *);
TCB* threadQueue_find_waiting_thread(ThreadQueue *, int);
TCB* threadQueue_highest_priority(ThreadQueue *);
void threadQueue_free(ThreadQueue *);

#endif