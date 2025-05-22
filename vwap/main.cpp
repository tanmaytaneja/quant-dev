#include "vwap.hpp"

#include <windows.h>
#include <iostream>
#include <x86intrin.h> //? For __rdtsc (works with GCC/Clang on x86/x64)

#define DEFAULT_WINDOW_TIME 30
#define DEFAULT_SIMULATION_TIME 50

template <typename PriceType>
__forceinline TickData<PriceType> generateSyntheticTick(int currentTime)
{
    return TickData<PriceType>(static_cast<PriceType>(100.0 + (currentTime & 0xF)), 1 + (currentTime % 5), currentTime);
}

int main()
{
    constexpr int NUM_TICKS = 1'000'000;

    std::cout << "VWAP Calculator" << std::endl;
    int windowTime = DEFAULT_WINDOW_TIME;
    int simulationTime = DEFAULT_SIMULATION_TIME;

    std::cout << "Simulation Start" << std::endl;
    std::cout << std::endl;

    VWAPCalculator<double> vWAPCalculator(windowTime);
    /* or
    #include <memory>
    auto vWAPCalculator = std::make_unique<VWAPCalculator>(windowTime);
    ? only use when dynamic lifetime of object is required.
    */

    unsigned long long start = __rdtsc();

    for (int currentTime = 0; currentTime < NUM_TICKS; ++currentTime)
    {
        TickData<double> syntheticTick = generateSyntheticTick<double>(currentTime);
        vWAPCalculator.insertTick(syntheticTick);
    }

    unsigned long long end = __rdtsc();

    std::cout << "Final VWAP: " << vWAPCalculator.getVWAP() << std::endl;
    std::cout << "Total CPU Cycles: " << end - start << std::endl;
    std::cout << "Average CPU Cycles per insertTick(): " << (end - start) / NUM_TICKS << std::endl;
    std::cout << std::endl;

    std::cout << "Simulation End" << std::endl;

    return 0;
}