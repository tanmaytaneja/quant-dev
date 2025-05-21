#ifndef VWAP_CPP
#define VWAP_CPP

#include <iostream>
#include <queue>

/* 16 Bytes */
struct TickData //TODO: struct cache aligning
{
    double price;
    int volume;
    int timestamp;
};

class VWAPCalculator
{
    public:
        void insertTick(const TickData tickData)
        {
            if (tickData.timestamp - VWP.front().timestamp >= windowSeconds)
            {
                VWAPSum -= VWP.front().price * VWP.front().volume;
                totalVolume -= VWP.front().volume;
                VWP.pop();
            }
            VWP.push(tickData);
            VWAPSum += tickData.price * tickData.volume;
            totalVolume += tickData.volume;
            VWAP = VWAPSum / totalVolume;
        }

        double getVWAP() const 
        {
            return VWAP;
        }

        VWAPCalculator(int windowSeconds) : windowSeconds{windowSeconds}, VWAP{0}, VWAPSum{0}, totalVolume{0}
        {
            //TODO: VWP queue resizing ?? it's according to timestamp though, so maybe size can be indefinite?
        }

    private:
    int windowSeconds;
    double VWAP;
    double VWAPSum;
    double totalVolume;
    std::queue<TickData> VWP;
};

#endif