#ifndef VWAP_HPP
#define VWAP_HPP

#define TICK_WINDOW_QUEUE 0

#include <iostream>
#include <queue>

static constexpr int MAX_TICKS = 1024;
static constexpr int TICK_MASK = MAX_TICKS - 1; //? Ensure MAX_TICKS is a power of 2

/* 16 Bytes */
template <typename PriceType>
struct alignas(32) TickData // TODO: struct cache aligning
{
    PriceType price;
    int volume;
    int timestamp;

    TickData() = default; //? Required when this struct is part of an array!
    TickData(PriceType price, int volume, int timestamp)
        : price{price}, volume{volume}, timestamp{timestamp} {}
};

template <typename PriceType>
class VWAPCalculator
{
public:
    __forceinline void insertTick(const TickData<PriceType> &tickData)
    {
#if TICK_WINDOW_QUEUE == 1
        while (!tickWindow.empty() && tickData.timestamp - tickWindow.front().timestamp >= windowSeconds)
        {
            const auto &old = tickWindow.front();
            VWAPSum -= old.price * old.volume;
            totalVolume -= old.volume;
            tickWindow.pop();
        }
        tickWindow.push(tickData);
        VWAPSum += tickData.price * tickData.volume;
        totalVolume += tickData.volume;
#else
        while (size && tickData.timestamp - tickWindow[head].timestamp >= windowSeconds)
        {
            const auto &old = tickWindow[head];
            VWAPSum -= old.price * old.volume;
            totalVolume -= old.volume;
            head = (head + 1) & MAX_TICKS;
            --size;
        }
        tickWindow[tail] = tickData;
        VWAPSum += tickData.price * tickData.volume;
        totalVolume += tickData.volume;
        tail = (tail + 1) & MAX_TICKS;
        ++size;
#endif
    }

    __forceinline double getVWAP() const
    {
        return VWAPSum / totalVolume;
    }

    VWAPCalculator(int windowSeconds)
        : windowSeconds{windowSeconds}, VWAPSum{0}, totalVolume{0}, head{0}, tail{0}, size{0}
    {
        // TODO: tickWindow resizing ?? it's according to timestamp though, so maybe size can be indefinite?
    }

    VWAPCalculator(const VWAPCalculator &) = delete;
    VWAPCalculator &operator=(const VWAPCalculator &) = delete;
    VWAPCalculator(VWAPCalculator &&) = delete;

private:
    int windowSeconds;
    double VWAPSum;
    int totalVolume;
#if TICK_WINDOW_QUEUE == 1
    std::queue<TickData<PriceType>> tickWindow; //! Never use std::queue in HFT since it is backed by std::deque, this causes dynamic allocations and non-contiguous memory.
#else
    TickData<PriceType> tickWindow[MAX_TICKS]; //? Use this instead. This represents a ring-buffer. It offers us ZERO dynamic memory allocations, constant time push/pop and predictable memory layout.
#endif
    size_t head;
    size_t tail;
    size_t size;
};

#endif