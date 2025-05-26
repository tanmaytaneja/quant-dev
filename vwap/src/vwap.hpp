#ifndef VWAP_HPP
#define VWAP_HPP

#define CUSTOM_RING_BUFFER 1

#include "macros.hpp"

#include <iostream>

#if CUSTOM_RING_BUFFER == 1
#include "ring-buffer.hpp"
#else
#include <queue>
#endif

static constexpr int MAX_TICKS = 1024;

/* 16 Bytes */
template <typename PriceType>
struct /* alignas(32) */ TickData // TODO: struct cache aligning
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
    FORCE_INLINE void insertTick(const TickData<PriceType> &tickData)
    {
        while (!tickWindow.empty() && tickData.timestamp - tickWindow.front().timestamp >= windowSeconds)
        {
            const auto &old = tickWindow.front();
            VWAPSum -= old.price * old.volume;
            totalVolume -= old.volume;
            tickWindow.pop();
        }
        VWAPSum += tickData.price * tickData.volume;
        totalVolume += tickData.volume;
        tickWindow.push(tickData);
    }

    FORCE_INLINE double getVWAP() const
    {
        return VWAPSum / totalVolume;
    }

    VWAPCalculator(int windowSeconds)
        : windowSeconds{windowSeconds}, VWAPSum{0}, totalVolume{0}
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
#if CUSTOM_RING_BUFFER == 1
    RingBuffer<TickData<PriceType>, MAX_TICKS> tickWindow; //? Use this instead. This represents a ring-buffer. It offers us ZERO dynamic memory allocations, constant time push/pop and predictable memory layout.
#else
    std::queue<TickData<PriceType>> tickWindow; //! Never use std::queue in HFT since it is backed by std::deque, this causes dynamic allocations and non-contiguous memory.
#endif
};

#endif