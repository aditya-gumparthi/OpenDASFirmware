#ifndef FIFO_HPP
#define FIFO_HPP
#include <stdint.h>
template <typename T, size_t N>
class Fifo
{
private:
    T arr[N];
    size_t front = 0;
    size_t rear = 0;
    size_t count = 0;

public:
    Fifo() { ; }

    bool enqueue(const T &item)
    {
        if (count >= N)
        {
            // Queue is full, cannot add more items
            return false;
        }

        arr[rear] = item;
        rear = (rear + 1) % N;
        count++;
        return true;
    }

    bool dequeue(T &item)
    {
        if (count == 0)
        {
            // Queue is empty, nothing to dequeue
            return false;
        }

        item = arr[front];
        front = (front + 1) % N;
        count--;
        return true;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    bool isFull() const
    {
        return count == N;
    }

    size_t num_items() const
    {
        return count;
    }

    size_t size() const
    {
        return N;
    }
};

#endif /* FIFO_HPP */
