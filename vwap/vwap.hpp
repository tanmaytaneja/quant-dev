#ifndef VWAP_HPP
#define VWAP_HPP

#include <iostream>
#include <queue>

/* 16 Bytes */
template <typename PriceType>
struct alignas(16) TickData // TODO: struct cache aligning
{
    PriceType price;
    int volume;
    int timestamp;
    // static_assert(sizeof(TickData<PriceType>) == 16, "struct TickData<PriceType> is not tightly packed!");
};

template <typename PriceType>
class VWAPCalculator
{
public:
    void insertTick(const TickData<PriceType> &tickData)
    {
        while (!tickWindow.empty() && tickData.timestamp - tickWindow.front().timestamp >= windowSeconds)
        {
            const auto& old = tickWindow.front();
            VWAPSum -= old.price * old.volume;
            totalVolume -= old.volume;
            tickWindow.pop();
        }
        tickWindow.push(tickData);
        VWAPSum += tickData.price * tickData.volume;
        totalVolume += tickData.volume;
    }

    double getVWAP() const
    {
        return VWAPSum / totalVolume;
    }

    VWAPCalculator(int windowSeconds) : windowSeconds{windowSeconds}, VWAPSum{0}, totalVolume{0}
    {
        // TODO: tickWindow queue resizing ?? it's according to timestamp though, so maybe size can be indefinite?
    }

    VWAPCalculator(const VWAPCalculator&) = delete;
    VWAPCalculator& operator=(const VWAPCalculator&) = delete;

private:
    int windowSeconds;
    double VWAPSum;
    int totalVolume;
    std::queue<TickData<PriceType>> tickWindow;
};

#endif