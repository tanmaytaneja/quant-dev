#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include "macros.hpp"

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

namespace TT
{
    template <typename T, size_t RequestedCapacity>
    class RingBuffer
    {
    public:
        RingBuffer() : head{0}, tail{0}, count{0}
        {
            static_assert((Capacity & Mask) == 0, "Capacity of RingBuffer must be a power of 2");
        }

        FORCE_INLINE bool empty() const noexcept
        {
            return !count;
        }

        FORCE_INLINE bool full() const noexcept
        {
            return count == Capacity;
        }

        FORCE_INLINE size_t size() const noexcept
        {
            return count;
        }

        FORCE_INLINE void push(const T &item) noexcept
        {
            buffer[tail] = item;
            tail = (tail + 1) & Mask;
            ++count;
        }

        FORCE_INLINE void push(const T &&item) noexcept
        {
            buffer[tail] = std::move(item);
            tail = (tail + 1) & Mask;
            ++count;
        }

        FORCE_INLINE void pop() noexcept
        {
            head = (head + 1) & Mask;
            --count;
        }

        FORCE_INLINE T &front() noexcept
        {
            return buffer[head];
        }

        FORCE_INLINE const T &front() const noexcept
        {
            return buffer[head];
        }

        FORCE_INLINE void clear() noexcept
        {
            head = tail = count = 0;
        }

    private:
        static constexpr size_t Capacity = next_power_of_two(RequestedCapacity);
        static constexpr size_t Mask = Capacity - 1;
        alignas(32) T buffer[Capacity];
        size_t head;
        size_t tail;
        size_t count;
    };
}
#endif

//* Instead of std::string, use char[N] or std::array<char, N> or custom SSO strings.
//! What are SSO strings?

//* Can remove count, since head - tail is the same thing. Moreover, the 'count' variable can be placed in a different cache line, which is more latency sensitive compared to subtraction (head - tail).

//* In tight, branchless HFT code, fewer memory read/writes are better than extra arithmetic.

//* if head and tail are present in the same cache line, then multiple threads accessing them may cause cache line thrashing since both those threads will interfere in each other's cache lines. This can impact latency. Therefore, it is beneficial to alignas(64) head and tail, so that they are not present in the same cache line.
//! but won't we be wasting cache lines then? What if only a single thread is using it? Then which one is better?
//? Yes, technically you are wasting memory — but only a tiny amount (e.g., a few bytes per 64-byte cache line).
//? But in HFT, you prioritize latency over memory. Cache line thrashing is far more costly than a little memory waste.

//* std::atomic<size_t> head and tail will make the queue good for SPSC (one pushes and one pops).
//! fails for SPMC, MPSC, MPMC. This will require (compare_and_swap / atomic_flags) / memory ordering control (memory_order_acquire / release)
//! can also use algos like disruptor or libraries like folly::MPMCQueue

//* __builtin_prefetch loads the memory in the cache line already before we use it!
//? __builtin_prefetch(const void* addr, int rw = 0, int locality = 3)
//? addr: address to prefetch, rw: 0(read)/1(write), locality: 0-3 (higher means data will be used more!)

//! Understand the entire virtual function + vtable thing!