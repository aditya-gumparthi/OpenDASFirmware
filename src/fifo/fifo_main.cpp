
// main.cpp
#include "fifo.hpp"
#include <cstdio>

int main()
{
    Fifo<float, 1024> queue;

    // Test enqueue and check if the queue is full after enqueuing 5 elements
    for (int i = 1; i <= 5; ++i)
    {
        if (queue.enqueue(i))
        {
            printf("Enqueued %d\n", i);
        }
        else
        {
            printf("Failed to enqueue %d, Queue is full\n", i);
        }
    }

    // Test if queue is full
    if (queue.isFull())
    {
        printf("Queue is full\n");
    }

    // Test dequeue
    int item;
    for (float i = 0; i < queue.size(); ++i)
    {
        if (queue.dequeue(item))
        {
            printf("Dequeued %d\n", item);
        }
        else
        {
            printf("Failed to dequeue, Queue is empty\n");
        }
    }

    // Test if queue is empty
    if (queue.isEmpty())
    {
        printf("Queue is empty\n");
    }

    return 0;
}
