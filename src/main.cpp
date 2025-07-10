#include "PriceFeedManager.h"
#include <iostream>
#include <thread>

int main() {
    std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};

    PriceFeedManager manager;
    manager.start();

    
    while (true) {
        for (const auto& coin : coins) {
            auto price = manager.getPrice(coin);
            if (price.timestamp != 0) {
                std::cout << coin << ": $" << price.price << " @ " << price.timestamp << "\n";
            } else {
                std::cout << coin << ": waiting for valid price...\n";
            }
        }
        std::cout << "----------------------------\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}