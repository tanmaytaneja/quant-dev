#ifndef MEDIAN_HPP
#define MEDIAN_HPP

#define CUSTOM_SORTED_ARRAY 0

#include "macros.hpp"
#include "ring-buffer.hpp"

#if CUSTOM_SORTED_ARRAY == 1
#define USE_SET_EXTRACT 0
#include "sorted-array.hpp"
#else
#define USE_SET_EXTRACT 1
#include <set>
#endif

/* 16 Bytes */
template <typename PriceType>
struct Tick // TODO: struct cache aligning
{
    PriceType price;
    int volume;
    int timestamp;

    Tick() = default; //? Required when this struct is part of an array!
    Tick(PriceType price, int volume, int timestamp)
        : price{price}, volume{volume}, timestamp{timestamp} {}

    bool operator<(const Tick &other) const
    {
        return price < other.price || (price == other.price && timestamp < other.timestamp);
    }

    bool operator==(const Tick &other) const
    {
        return /* price == other.price && volume == other.volume && */ timestamp == other.timestamp;
    }
};

template <typename PriceType>
class MedianCalculator
{
public:
    MedianCalculator(int windowSeconds)
        : windowSeconds{windowSeconds}, cumulativeLowerVolume{0}, cumulativeUpperVolume{0}, cumulativeVolume{0}
    {
        // TODO: Constructor shit
    }

    FORCE_INLINE void removeTick(const Tick<PriceType> &tick)
    {
        cumulativeVolume -= tick.volume;
#if CUSTOM_SORTED_ARRAY == 1
        
#else
#endif
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
            upperTicks.erase(tick); //! Does this waste extra time? Can we just search up the iterator once?
        }
        ticks.pop();
    }

    FORCE_INLINE void insertTick(const Tick<PriceType> &tick)
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

        if (cumulativeLowerVolume && tick.price > std::prev(lowerTicks.end())->price)
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

    FORCE_INLINE void balanceTicks()
    {
        while (cumulativeLowerVolume < ((cumulativeVolume + 1) >> 1))
        {
#if USE_SET_EXTRACT == 1
            auto upperTick = upperTicks.extract(upperTicks.begin());
            cumulativeUpperVolume -= upperTick.value().volume;
            cumulativeLowerVolume += upperTick.value().volume;
            lowerTicks.insert(std::move(upperTick));
#else
            auto upperTick = upperTicks.begin();
            upperTicks.erase(upperTick);
            cumulativeUpperVolume -= upperTick->volume;
            lowerTicks.insert(*upperTick);
            cumulativeLowerVolume += upperTick->volume;
#endif
        }
    }

    FORCE_INLINE PriceType getMedian() const
    {
        return std::prev(lowerTicks.end())->price;
    }

    MedianCalculator(const MedianCalculator &) = delete;
    MedianCalculator &operator=(const MedianCalculator &) = delete;
    MedianCalculator(MedianCalculator &&) = delete;

private:
    int windowSeconds;
    int cumulativeLowerVolume;
    int cumulativeUpperVolume;
    int cumulativeVolume;
    static constexpr size_t MAX_TICKS = 4096;
    RingBuffer<Tick<PriceType>, MAX_TICKS> ticks;
#if CUSTOM_SORTED_ARRAY == 1
    SortedArray<Tick<PriceType>, MAX_TICKS> lowerTicks;
    SortedArray<Tick<PriceType>, MAX_TICKS> upperTicks;
#else
    std::set<Tick<PriceType>> lowerTicks;
    std::set<Tick<PriceType>> upperTicks;
#endif
};

#endif