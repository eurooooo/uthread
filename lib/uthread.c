#include <string.h>
#include <signal.h>

#include "uthread.h"
#include "ustructs.h"

schedulerType stype = RR;
int aging_rate = 0;
int quanta_left = 0;
TCB *t_running = NULL;
TCB *threads[MAX_THREADS] = {0};

ThreadQueue ready_queue;
ThreadQueue finished_queue;
ThreadQueue join_queue;
ThreadQueue blocked_queue;

sigset_t timerInterrupt;
struct itimerval timer;

/******************************/
/*     Context Switch         */
/******************************/

int switchThreads()
{
    // Adjust priorities
    if (stype == DP)
    {
        adjust_priorities();
    }

    // Get new thread from scheduler
    int tid = scheduler();
    volatile int flag = 0;
    // Save previous's thread state
    if (getcontext(&threads[uthread_self()]->context_) == -1)
    {
        return -1;
    }

    if (flag == 1) // handle double return
        return 0;

    flag = 1;

    // Set running thread
    t_running = threads[tid];
    t_running->state_ = RUNNING;
    // Reset the timer so the new thread gets a full quantum
    if (resetTimer() == -1)
    {
        return -1;
    }
    // Load in the new thread
    if (setcontext(&threads[tid]->context_) == -1)
    {
        return -1;
    }

    // It should not be possible to reach this line
    return 0;
}

/******************************/
/*           Timer            */
/******************************/

int blockTimer()
{
    // Block SIGVTALRM
    int ret = sigprocmask(SIG_BLOCK, &timerInterrupt, NULL);
    if (ret == -1)
    {
        perror("sigprocmask");
        return -1;
    }

    return 0;
}

int unblockTimer()
{
    // Unblock SIGVTALRM
    int ret = sigprocmask(SIG_UNBLOCK, &timerInterrupt, NULL);
    if (ret == -1)
    {
        perror("sigprocmask");
        return -1;
    }

    return 0;
}

int startTimer()
{
    // Start timer
    int ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);
    if (ret == -1)
    {
        perror("setitimer");
        return -1;
    }

    return 0;
}

int resetTimer()
{
    // The timer can be reset the same way it is set
    return startTimer();
}

void timerHandler(int sig)
{
    // Remove running process to the ready queue
    if (t_running && t_running->state_ == RUNNING)
    {
        t_running->state_ = READY;
        if (threadQueue_enque(&ready_queue, t_running) == -1)
        {
            exit(-1);
        }
    }

    // Load new process
    switchThreads();
}

/******************************/
/*         Scheduler          */
/******************************/

int scheduler()
{
    TCB *next_thread;

    // Round Robin
    if (stype == RR)
    {
        next_thread = threadQueue_deque(&ready_queue);
    }

    // Static Priority and Dynamic Priority
    if (stype == SP || stype == DP)
    {
        next_thread = threadQueue_highest_priority(&ready_queue);
        threadQueue_delete(&ready_queue, next_thread);
    }

    return next_thread->id_;
}

/************************************/
/*          ULT APIs                */
/************************************/

int uthread_init(char *scheduler, int quantum, ...)
{
    // Set the scheduler
    if (scheduler == NULL)
    {
        fprintf(stderr, "No scheduler\n");
        return -1;
    }
    else if (strcmp(scheduler, "RR") == 0)
    {
        stype = RR;
    }
    else if (strcmp(scheduler, "SP") == 0)
    {
        stype = SP;
    }
    else if (strcmp(scheduler, "DP") == 0)
    {
        stype = DP;
    }
    else
    {
        fprintf(stderr, "Wrong scheduler type is given.\n");
        return -1;
    }

    // Initialize any scheduler specific values
    if (stype == DP)
    {
        va_list args;
        va_start(args, quantum);
        aging_rate = va_arg(args, int);
        va_end(args);
        quanta_left = aging_rate;
    }

    // Initialize required queues
    threadQueue_init(&ready_queue);
    threadQueue_init(&finished_queue);
    threadQueue_init(&join_queue);
    threadQueue_init(&blocked_queue);

    // Initialize main thread TCB and set it as running
    TCB *mainthread_TCB = (TCB *)malloc(sizeof(TCB));
    if (mainthread_TCB == NULL)
    {
        perror("malloc");
        return -1;
    }

    mainthread_TCB->id_ = MAIN_THREAD;
    mainthread_TCB->state_ = RUNNING;
    mainthread_TCB->priority_ = MAX_PRIORITY;
    mainthread_TCB->stack_ = NULL;
    mainthread_TCB->timeWaiting = 0;

    t_running = mainthread_TCB;
    threads[MAIN_THREAD] = mainthread_TCB;

    // Initialize global timer length
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = quantum;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum;

    // Initialize global timer interupt set
    int ret = sigemptyset(&timerInterrupt);
    if (ret == -1)
    {
        perror("sigemptyset");
        return -1;
    }

    ret = sigaddset(&timerInterrupt, SIGVTALRM);
    if (ret == -1)
    {
        perror("sigaddset");
        return -1;
    }

    // Add timer interupt handler
    struct sigaction timeraction;
    memset(&timeraction, 0, sizeof(timeraction));
    timeraction.sa_handler = &timerHandler;
    timeraction.sa_flags = 0;
    timeraction.sa_mask = timerInterrupt;

    ret = sigaction(SIGVTALRM, &timeraction, NULL);
    if (ret == -1)
    {
        perror("sigaction");
        return -1;
    }

    // Start Timer
    if (startTimer() == -1)
    {
        return -1;
    }

    return 0;
}

int get_new_tid()
{
    // Search thread array for first available index
    for (int i = 1; i < MAX_THREADS; i++)
    {
        if (threads[i] == NULL)
        {
            return i;
        }
    }
    // Return error if the array is full
    return -1;
}

int uthread_create(void(*start_routine), void *arg, ...)
{
    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return -1;
    }

    // Generate thread ID
    int thread_ID = get_new_tid();
    if (thread_ID == -1)
    {
        goto unblock;
    }

    // Create thread control block for new thread
    TCB *thread_control_block = malloc(sizeof(TCB));
    if (thread_control_block == NULL)
    {
        goto unblock;
    }

    // Allocate memory for the new stack
    char *thread_stack = malloc(STACK_SZ);
    if (thread_stack == NULL)
    {
        goto freeTCB;
    }

    // Initialize TCB
    thread_control_block->id_ = thread_ID;
    thread_control_block->state_ = READY;
    thread_control_block->stack_ = thread_stack;
    thread_control_block->priority_ = 0;
    thread_control_block->timeWaiting = 0;

    // Get priority from va_args
    if (stype != RR)
    {
        va_list args;
        va_start(args, arg);
        int priority = va_arg(args, int);
        va_end(args);

        // Bound priority
        if (priority > MAX_PRIORITY)
        {
            priority = MAX_PRIORITY;
        }
        else if (priority < MIN_PRIORITY)
        {
            priority = MIN_PRIORITY;
        }

        thread_control_block->priority_ = priority;
    }

    // Make the context of TCB with required routine and arguments
    if (getcontext(&thread_control_block->context_) == -1)
    {
        goto freeStack;
    }

    thread_control_block->context_.uc_stack.ss_sp = thread_stack;
    thread_control_block->context_.uc_stack.ss_size = STACK_SZ;
    thread_control_block->context_.uc_stack.ss_flags = 0;

    makecontext(&thread_control_block->context_, (void (*)(void))stub, 2, start_routine, arg);

    // Enqueue to READY
    if (threadQueue_enque(&ready_queue, thread_control_block) == -1)
    {
        goto freeStack;
    }

    // Enable timer interrupts
    if (unblockTimer() == -1)
    {
        free(thread_stack);
        free(thread_control_block);
        return -1;
    }

    // Place thread in thread array
    threads[thread_ID] = thread_control_block;

    return thread_ID;

freeStack:
    free(thread_stack);
freeTCB:
    free(thread_control_block);
unblock:
    unblockTimer();

    return -1;
}

int uthread_join(int tid)
{
    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return -1;
    }

    TCB *target_thread = threads[tid];
    // Check if thread exists
    if (target_thread == NULL)
    {
        unblockTimer();
        return -1;
    }

    // Check if thread with tid has finished execution
    while (target_thread->state_ != FINISHED)
    {
        // If not, the current thread will have to wait till the tid thread is finished
        t_running->state_ = JOINED;
        t_running->waiting = tid;
        if (threadQueue_enque(&join_queue, t_running) == -1)
        {
            unblockTimer();
            return -1;
        }

        // Schedule a new thread (block until thread has finished)
        if (switchThreads() == -1)
        {
            unblockTimer();
            return -1;
        }
    }

    // Free resources
    free_thread(tid);

    // Enable timer interrupts again
    if (unblockTimer() == -1)
    {
        return -1;
    }

    return 0;
}

int uthread_yield()
{
    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return -1;
    }

    // Change the state to ready
    t_running->state_ = READY;

    // Enqueue the running thread into the ready queue
    if (threadQueue_enque(&ready_queue, t_running) == -1)
    {
        unblockTimer();
        return -1;
    }

    // Schedule a new thread
    if (switchThreads() == -1)
    {
        unblockTimer();
        return -1;
    }

    // Enable the timer again
    if (unblockTimer() == -1)
    {
        return -1;
    }

    return 0;
}

void uthread_exit(void)
{
    // Check if the main thread is exiting
    if (uthread_self() == MAIN_THREAD)
    {
        uthread_free();

        exit(0);
    }

    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return;
    }

    // If there are any threads waiting for current thread to exit, move it to the ready queue.
    TCB *waiting_thread = threadQueue_find_waiting_thread(&join_queue, uthread_self());
    if (waiting_thread)
    {
        waiting_thread->state_ = READY;
        threadQueue_delete(&join_queue, waiting_thread);
        if (threadQueue_enque(&ready_queue, waiting_thread) == -1)
        {
            return;
        }
    }

    // Enqueue current thread into finished_queue
    t_running->state_ = FINISHED;
    if (threadQueue_enque(&finished_queue, t_running) == -1)
    {
        return;
    }

    // Schedule a new thread
    if (switchThreads() == -1)
    {
        unblockTimer();
        return;
    }

    // Enable timer again
    if (unblockTimer() == -1)
    {
        return;
    }
}

int uthread_suspend(int tid)
{
    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return -1;
    }

    // Get the thread
    TCB *thread = threads[tid];
    if (thread == NULL || thread->state_ == FINISHED)
    {
        unblockTimer();
        return -1;
    }

    // Nothing to do if thread is already blocked
    if (thread->state_ == BLOCKED)
    {
        if (unblockTimer() == -1)
        {
            return -1;
        }
        return 0;
    }
    // Remove from ready queue
    else if (thread->state_ == READY)
    {
        threadQueue_delete(&ready_queue, thread);
    }
    // Remove from joined queue
    else if (thread->state_ == JOINED)
    {
        threadQueue_delete(&join_queue, thread);
    }

    // Set thread state to blocked and add to the blocked queue
    threads[tid]->state_ = BLOCKED;
    if (threadQueue_enque(&blocked_queue, threads[tid]) == -1)
    {
        unblockTimer();
        return -1;
    }

    // Context switch may be required if the thread is suspending itself
    if (tid == uthread_self())
    {
        // Schedule a new thread
        if (switchThreads() == -1)
        {
            unblockTimer();
            return -1;
        }
    }

    // Enable the timer again
    if (unblockTimer() == -1)
    {
        return -1;
    }

    return 0;
}

int uthread_resume(int tid)
{
    // Disable timer interrupts while modifying shared data
    if (blockTimer() == -1)
    {
        return -1;
    }

    // Get the thread
    TCB *thread = threads[tid];
    if (thread == NULL)
    {
        unblockTimer();
        return -1;
    }

    // Nothing to do if the thread isn't blocked
    if (thread->state_ != BLOCKED)
    {
        if (unblockTimer() == -1)
        {
            return -1;
        }
        return 0;
    }

    // Remove from blocked queue
    threadQueue_delete(&blocked_queue, thread);

    // Set thread state to ready and add to the ready queue
    threads[tid]->state_ = READY;
    if (threadQueue_enque(&ready_queue, threads[tid]) == -1)
    {
        unblockTimer();
        return -1;
    }

    // Enable timer again
    if (unblockTimer() == -1)
    {
        return -1;
    }

    return 0;
}

int uthread_self()
{
    // Return current thread's id
    if (t_running)
    {
        return t_running->id_;
    }
    return -1;
}

int uthread_priority(int tid)
{
    // Return a certain thread's priority
    if (threads[tid])
    {
        return threads[tid]->priority_;
    }
    return -1;
}

void uthread_free()
{
    threadQueue_free(&ready_queue);
    threadQueue_free(&blocked_queue);
    threadQueue_free(&join_queue);
    threadQueue_free(&finished_queue);
    for (int i = MAIN_THREAD + 1; i < MAX_THREADS; i++)
    {
        if (threads[i])
        {
            free(threads[i]->stack_);
            free(threads[i]);
            threads[i] = NULL;
        }
    }
    free(threads[MAIN_THREAD]);
}

void stub(void (*start_routine)(void *), void *arg)
{
    // Enable timer interupts on this thread
    if (unblockTimer() == -1)
    {
        return;
    }

    // Run the start_routine with argument arg
    start_routine(arg);

    // Call uthread_exit to clean up
    uthread_exit();
}

void print_ready_queue()
{
    // Currently consumes queue
    printf("READY QUEUE\n");
    TCB *thread;
    while ((thread = threadQueue_deque(&ready_queue)))
    {
        printf("ULT...[%d]\n", thread->id_);
    }
}

void free_thread(int tid)
{
    TCB *target_thread = threads[tid];

    // Remove from queue
    threadQueue_delete(&finished_queue, target_thread);

    // Free dynamically allocated memory
    free(target_thread->stack_);
    free(target_thread);

    // Free thread id
    threads[tid] = NULL;

    return;
}

void adjust_priorities()
{
    // Loop through the thread array
    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (threads[i])
        {
            // Penalize thread for running
            if (threads[i] == t_running)
            {
                // Reset count when thread gets a turn to run
                if (t_running->timeWaiting > 0)
                {
                    t_running->timeWaiting = 0;
                }
                // Record one quanta to the threads account
                t_running->timeWaiting--;
            }
            // Reward thread for waiting
            else if (threads[i]->state_ == READY)
            {
                // Record time spent waiting
                threads[i]->timeWaiting++;
            }

            // Increase Priority if necessary
            if (threads[i]->timeWaiting >= aging_rate)
            {
                threads[i]->timeWaiting = 0;
                if (threads[i]->priority_ > MIN_PRIORITY)
                {
                    threads[i]->priority_--;
                }
            }
            // Decrease Priority if necessary
            if (threads[i]->timeWaiting <= -aging_rate)
            {
                threads[i]->timeWaiting = 0;
                if (threads[i]->priority_ < MAX_PRIORITY)
                {
                    threads[i]->priority_++;
                }
            }
        }
    }
}