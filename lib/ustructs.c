#include <string.h>

#include "ustructs.h"

// Initialize the queue
void threadQueue_init(ThreadQueue *queue)
{
    queue->front = NULL;
    queue->back = NULL;
    queue->size = 0;
}

void threadQueue_free(ThreadQueue *queue)
{
    // Free all the nodes in the queue
    QueueNode *node = queue->front;
    while(node)
    {
        QueueNode *next = node->next;
        free(node);
        node = next;
    }

    queue->front = NULL;
    queue->back = NULL;
    queue->size = 0;
}

// Enqueue a thread
int threadQueue_enque(ThreadQueue *queue, TCB *thread)
{
    // Allocate new node
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (newNode == NULL)
    {
        perror("malloc");
        return -1;
    }

    // Initialize node
    newNode->thread = thread;
    newNode->next = NULL;
    newNode->prev = queue->back;

    // Link former last node
    if (queue->back)
    {
        queue->back->next = newNode;
    }

    // Place node into queue
    queue->back = newNode;
    if (queue->front == NULL)
    {
        queue->front = newNode;
    }

    queue->size++;

    return 0;
}

// Dequeue a thread
TCB *threadQueue_deque(ThreadQueue *queue)
{
    // Check if queue is empty
    if (queue->front == NULL)
    {
        return NULL;
    }

    // Get front node and its thread
    QueueNode *frontNode = queue->front;
    TCB *thread = frontNode->thread;

    // Relink front node of the queue
    queue->front = frontNode->next;
    if (queue->front)
    {
        queue->front->prev = NULL;
    }
    else
    {
        // The queue is now empty
        queue->back = NULL;
    }

    // Free node memory
    free(frontNode);

    queue->size--;

    return thread;
}

// Delete a specific thread from the queue
void threadQueue_delete(ThreadQueue *queue, TCB *thread)
{
    // Iterator pointer
    QueueNode *current = queue->front;
    while (current != NULL)
    {
        // Check for the target thread
        if (current->thread == thread)
        {
            // Check if there is a node before current
            if (current->prev)
            {
                current->prev->next = current->next;
            }
            // This is the front node
            else
            {
                queue->front = current->next;
            }

            // Check if there is a node after current
            if (current->next)
            {
                current->next->prev = current->prev;
            }
            // This is the back node
            else
            {
                queue->back = current->prev;
            }

            // Free node memory
            free(current);

            queue->size--;

            return;
        }
        // Move through the queue
        current = current->next;
    }
}

TCB *threadQueue_find_waiting_thread(ThreadQueue *queue, int tid)
{
    // Loop through the queue
    QueueNode *node = queue->front;
    while (node)
    {
        // Return the thread if it has the right waiting value
        if (node->thread->waiting == tid)
        {
            return node->thread;
        }

        node = node->next;
    }

    return NULL;
}

TCB *threadQueue_highest_priority(ThreadQueue *queue)
{
    int min_priority = MAX_PRIORITY;

    // Empty queue
    if (queue->size == 0)
    {
        return NULL;
    }

    QueueNode *node = queue->front;

    // Get the min priority in the queue
    while (node)
    {
        if (node->thread->priority_ < min_priority)
        {
            min_priority = node->thread->priority_;
        }

        node = node->next;
    }

    // Reset node to the front of the queue
    node = queue->front;
    // Find the thread with the highest priority
    while (node)
    {
        if (node->thread->priority_ == min_priority)
        {
            return node->thread;
        }

        node = node->next;
    }

    return NULL;
}
