#ifndef MEDIAN_HPP
#define MEDIAN_HPP

/* 16 Bytes */
template <typename PriceType>
struct alignas(32) Tick // TODO: struct cache aligning
{
    PriceType price;
    int volume;
    int timestamp;

    Tick() = default; //? Required when this struct is part of an array!
    Tick(PriceType price, int volume, int timestamp)
        : price{price}, volume{volume}, timestamp{timestamp} {}
};

template <typename PriceType>
class MedianCalculator
{
public:
    MedianCalculator(int windowSeconds)
        : windowSeconds{windowSeconds}, cumulativeLowerVolume{0}, cumulativeUpperVolume{0}, cumulativeVolume{0}
    {
    }

    __forceinline void removeTick(const Tick<PriceType> &tick)
    {
        cumulativeVolume -= tick.volume;
        if (upperTicks.find(tick) == upperTicks.end())
        {
            //? implies lower tick
            cumulativeLowerVolume -= tick.volume;
            lowerTicks.erase(tick);
        }
        else
        {
            //? implies upper tick
            cumulativeUpperVolume -= tick.volume;
            upperTicks.erase(tick);
        }
        ticks.erase(tick);
    }

    __forceinline void insertTick(const Tick<PriceType> &tick)
    {
        int currentTime = tick.timestamp;
        //? remove all ticks with timestamp less than N minutes from currentTime;
        while (!ticks.empty())
        {
            auto &old = ticks.front();
            if (currentTime - old.timestamp >= windowSeconds)
            {
                removeTick(old);
            }
            else
            {
                break;
            }
        }

        if (cumulativeLowerVolume && tick.price > *lowerTicks.end()->price)
        {
            upperTicks.insert(tick);
            cumulativeUpperVolume += tick.volume;
        }
        else
        {
            lowerTicks.insert(tick);
            cumulativeLowerVolume += tick.volume;
        }
        ticks.push(tick);
        cumulativeVolume += tick.volume;

        balanceTicks();
    }

    __forceinline void balanceTicks()
    {
        while (cumulativeLowerVolume < (cumulativeVolume >> 1))
        {
            auto upperTick = upperTicks.begin();
            upperTicks.erase(upperTick);
            cumulativeUpperVolume -= upperTick->volume;
            lowerTicks.insert(lowerTick);
            cumulativeLowerVolume += upperTick->volume;
        }
    }

    __forceinline PriceType getMedian() const
    {
        return *lowerTicks.end()->price;
    }

    MedianCalculator(const MedianCalculator &) = delete;
    MedianCalculator &operator=(const MedianCalculator &) = delete;
    MedianCalculator(MedianCalculator &&) = delete;

private:
    int windowSeconds;
    int cumulativeLowerVolume;
    int cumulativeUpperVolume;
    int cumulativeVolume;
    std::queue<Tick<PriceType>> ticks;
    //! need to store the cumulative volume on both sides. Can be a little complex.
    std::set<Tick<PriceType>> lowerTicks;
    std::set<Tick<PriceType>> upperTicks;
};

#endif