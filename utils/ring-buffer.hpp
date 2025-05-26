#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <iostream>

constexpr size_t next_power_of_two(size_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if constexpr (sizeof(size_t) == 8)
    {
        n |= n >> 32;
    }
    return n + 1;
}

template <typename T, size_t RequestedCapacity>
class RingBuffer
{
public:
    RingBuffer() : head{0}, tail{0}, count{0}
    {
        static_assert((Capacity & Mask) == 0, "Capacity of RingBuffer must be a power of 2");
    }

    __forceinline bool empty() const
    {
        return !count;
    }

    __forceinline bool full() const
    {
        return count == Capacity;
    }

    __forceinline size_t size() const
    {
        return count;
    }

    __forceinline void push(const T &item)
    {
        buffer[tail] = item;
        tail = (tail + 1) & Mask;
        ++count;
    }

    __forceinline void pop()
    {
        head = (head + 1) & Mask;
        --count;
    }

    __forceinline T &front()
    {
        return buffer[head];
    }

    __forceinline const T &front() const
    {
        return buffer[head];
    }

private:
    static constexpr size_t Capacity = next_power_of_two(RequestedCapacity);
    static constexpr size_t Mask = Capacity - 1;
    T buffer[Capacity];
    size_t head;
    size_t tail;
    size_t count;
};

#endif