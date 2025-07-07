#include "PriceFeedManager.h"
#include <iostream>
#include <thread>

int main() {
    PriceFeedManager manager;
    manager.start();

    while (true) {
        auto price = manager.getPrice("usdc");
        if (price.timestamp != 0) {
            std::cout << "USDC: $" << price.price << " @ " << price.timestamp << "\n";
        } else {
            std::cout << "Waiting for valid price...\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}