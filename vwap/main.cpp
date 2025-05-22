#include "vwap.hpp"

#include <iostream>

#define DEFAULT_WINDOW_TIME 30
#define DEFAULT_SIMULATION_TIME 50

template <typename PriceType>
TickData<PriceType> generateRandomTick(int &currentTime)
{
    PriceType price{0};
    int volume{0};
    std::cout << "Price(" << ++currentTime << "): ";
    std::cin >> price;
    std::cout << "Volume(" << currentTime << "): ";
    std::cin >> volume;
    return TickData<PriceType>{price, volume, currentTime};
}

int main()
{
    std::cout << "VWAP Calculator" << std::endl;
    int currentTime = 0;
    int windowTime = DEFAULT_WINDOW_TIME;
    int simulationTime = DEFAULT_SIMULATION_TIME;
    std::cout << "Window Time (in seconds): ";
    std::cin >> windowTime;
    std::cout << "Simulation Time (in seconds): ";
    std::cin >> simulationTime;

    std::cout << "Simulation Start" << std::endl;

    VWAPCalculator<double> vWAPCalculator(windowTime);
    /* or
    #include <memory>
    auto vWAPCalculator = std::make_unique<VWAPCalculator>(windowTime);
    ? only use when dynamic lifetime of object is required.
    */

    while (simulationTime--)
    {
        std::cout << std::endl;
        vWAPCalculator.insertTick(generateRandomTick<double>(currentTime));
        std::cout << "Current VWAP: " << vWAPCalculator.getVWAP() << std::endl
                  << std::endl;
    }

    std::cout << "Simulation End" << std::endl;

    return 0;
}