#include "PriceFeedManager.h"
#include "RiskEngine.h"
#include "HistoricalPriceTracker.h"
#include <iostream>
#include <thread>

int main() {
    std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};

    PriceFeedManager manager;
    manager.start();

    RiskEngine risk(0.995, 0.990);

    HistoricalPriceTracker historyTracker;

    
    while (true) {
        for (const auto& coin : coins) {
            auto price = manager.getPrice(coin);
            if (price.timestamp != 0) {
                historyTracker.addPrice(coin, price.price);
                RiskLevel level = risk.assessRisk(price.price);
                auto stats = historyTracker.analyze(coin);
                std::cout << coin << ": $" << price.price << " @ " << price.timestamp
                          << " → Risk: " << risk.riskToString(level);
                          if (stats.trendingDown) std::cout << " , 📉 trending down";
                          if (stats.stddev > 0.002) std::cout << " , ⚠️ volatile";
                          std::cout << "\n";
            } else {
                std::cout << coin << ": waiting for valid price...\n";
            }
        }
        std::cout << "----------------------------\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}