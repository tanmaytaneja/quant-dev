#include "vwap.cpp"

#include <iostream>

#define DEFAULT_WINDOW_TIME 30
#define DEFAULT_SIMULATION_TIME 50

int currentTimestamp = 0;

TickData generateRandomTick()
{
    double price{0};
    int volume{0};
    std::cout << "Price(" << ++currentTimestamp << "): ";
    std::cin >> price;
    std::cout << "Volume(" << currentTimestamp << "): ";
    std::cin >> volume;
    return TickData{price, volume, currentTimestamp};
}

int main()
{
    std::cout << "VWAP Calculator" << std::endl;
    int windowTime = DEFAULT_WINDOW_TIME;
    int simulationTime = DEFAULT_SIMULATION_TIME;
    std::cout << "Window Time (in seconds): ";
    std::cin >> windowTime;
    std::cout << "Simulation Time (in seconds): ";
    std::cin >> simulationTime;

    std::cout << "Simulation Start" << std::endl;

    VWAPCalculator* vWAPCalculator = new VWAPCalculator(windowTime);

    while(simulationTime--)
    {
        std::cout << std::endl;
        vWAPCalculator->insertTick(generateRandomTick());
        std::cout << "Current VWAP: " << vWAPCalculator->getVWAP() << std::endl << std::endl;
    }

    std::cout << "Simulation End" << std::endl;

    return 0;
}