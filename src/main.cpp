#include "PriceFeedManager.h"
#include <iostream>
#include <thread>

int main() {
    PriceFeedManager manager;
    manager.start();

    while (true) {
        auto price = manager.getPrice("usdc");
        std::cout << "USDC: $" << price.price << " @ " << price.timestamp << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}