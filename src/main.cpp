#include "PriceFeedManager.h"
#include "RiskEngine.h"
#include <iostream>
#include <thread>

int main() {
    std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};

    PriceFeedManager manager;
    manager.start();

    RiskEngine risk(0.995, 0.990);

    
    while (true) {
        for (const auto& coin : coins) {
            auto price = manager.getPrice(coin);
            if (price.timestamp != 0) {
                RiskLevel level = risk.assessRisk(price.price);
                std::cout << coin << ": $" << price.price << " @ " << price.timestamp
                          << " → Risk: " << risk.riskToString(level) << "\n";
            } else {
                std::cout << coin << ": waiting for valid price...\n";
            }
        }
        std::cout << "----------------------------\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}