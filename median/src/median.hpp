#ifndef MEDIAN_HPP
#define MEDIAN_HPP

#define CUSTOM_SORTED_ARRAY 1

#include "macros.hpp"
#include "ring-buffer.hpp"

#include <iostream>

#if CUSTOM_SORTED_ARRAY == 1
#include "sorted-array.hpp"
#else
#include <set>
#endif

/* 16 Bytes */
template <typename PriceType>
struct Tick // TODO: struct cache aligning
{
    PriceType price;
    int volume;
    int timestamp;

    Tick() noexcept = default; //? Required when this struct is part of an array!
    Tick(PriceType price, int volume, int timestamp) noexcept
        : price{price}, volume{volume}, timestamp{timestamp} {}

    bool operator<(const Tick &other) const noexcept
    {
        return price < other.price || (price == other.price && timestamp < other.timestamp);
    }

    bool operator==(const Tick &other) const noexcept
    {
        return /* price == other.price && volume == other.volume && */ timestamp == other.timestamp;
    }
};

template <typename PriceType>
std::ostream &operator<<(std::ostream &os, const Tick<PriceType> &tick)
{
    return os << "[" << tick.price << ", " << tick.volume << ", " << tick.timestamp << "]";
}

template <typename PriceType>
class MedianCalculator
{
public:
    MedianCalculator(int windowSeconds)
        : windowSeconds{windowSeconds}, cumulativeLowerVolume{0}, cumulativeUpperVolume{0}, cumulativeVolume{0}
    {
        // TODO: Constructor shit
    }

    FORCE_INLINE void removeTick(const Tick<PriceType> &tick) noexcept
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
            upperTicks.erase(tick); //! Does this waste extra time? Can we just search up the iterator once?
        }
        ticks.pop();
    }

    //TODO: Add Templatized/Compile-Time Lazy Balancing Option (insert 10K ticks, getMedian once each second)
    FORCE_INLINE void insertTick(const Tick<PriceType> &tick) noexcept
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
            cumulativeUpperVolume += tick.volume;
            upperTicks.insert(tick);
        }
        else
        {
            cumulativeLowerVolume += tick.volume;
            lowerTicks.insert(tick);
        }
        ticks.push(tick);
        cumulativeVolume += tick.volume;

        balanceTicks();
    }

    FORCE_INLINE void balanceTicks() noexcept
    {
        while (cumulativeLowerVolume < ((cumulativeVolume + 1) >> 1))
        {
            auto upperTick = upperTicks.extract(upperTicks.begin());
#if CUSTOM_SORTED_ARRAY == 1
            cumulativeUpperVolume -= upperTick.volume;
            cumulativeLowerVolume += upperTick.volume;
#else
            cumulativeUpperVolume -= upperTick.value().volume;
            cumulativeLowerVolume += upperTick.value().volume;
#endif
            lowerTicks.insert(std::move(upperTick));
        }
    }

    FORCE_INLINE PriceType getMedian() const noexcept
    {
#if CUSTOM_SORTED_ARRAY == 1
        return lowerTicks.back().price;
#else
        return std::prev(lowerTicks.end())->price;
#endif
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