#include "median.hpp"

#include <windows.h>
#include <iostream>
#include <x86intrin.h> //? For __rdtsc (works with GCC/Clang on x86/x64)

#define DEFAULT_WINDOW_TIME 5
#define DEFAULT_SIMULATION_TIME 50

#define LOG 0

/* Flags to use while compiling and linking: -O3 -flto */

template <typename PriceType>
__forceinline Tick<PriceType> generateSyntheticTick(int currentTime)
{
    return Tick<PriceType>(static_cast<PriceType>(100.0 + (currentTime & 0xF)), 1 + (currentTime % 5), currentTime);
}

int main()
{
    constexpr int NUM_TICKS = 10;

    std::cout << "VW Median Calculator" << std::endl;
    int windowTime = DEFAULT_WINDOW_TIME;
    int simulationTime = DEFAULT_SIMULATION_TIME;

    std::cout << "Simulation Start" << std::endl;
    std::cout << std::endl;

    MedianCalculator<double> medianCalculator(windowTime);

    unsigned long long start = __rdtsc();

    for (int currentTime = 0; currentTime < NUM_TICKS; ++currentTime)
    {
        Tick<double> syntheticTick = generateSyntheticTick<double>(currentTime);
        medianCalculator.insertTick(syntheticTick);
#if LOG == 1
        std::cout << "Current Tick: Price(" << syntheticTick.price << ") Volume (" << syntheticTick.volume << ") Timestamp (" << syntheticTick.timestamp << ")" << std::endl;
        std::cout << "Current VW Median: " << medianCalculator.getMedian() << std::endl;
#endif
    }

    unsigned long long end = __rdtsc();

    std::cout << "Final VW Median: " << medianCalculator.getMedian() << std::endl;
    std::cout << "Total CPU Cycles (for generateSyntheticTick + insertTick): " << end - start << std::endl;
    std::cout << "Average CPU Cycles per (generateSyntheticTick + insertTick): " << (end - start) / NUM_TICKS << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation End" << std::endl;

    return 0;
}