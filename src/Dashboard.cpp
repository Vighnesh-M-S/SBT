// Dashboard.cpp
#include "Dashboard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape to clear terminal
}

void showDashboard(const std::string& csvPath, PriceFeedManager& manager) {
    manager.start();
    clearScreen();

    // Timestamp
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "🕒  " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "\n";

    // Stablecoin Prices
    std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};
    std::cout << "\n💱 Stablecoin Prices:\n";
    for (const auto& coin : coins) {
        auto price = manager.getPrice(coin);
        std::cout << "  - " << std::setw(5) << std::left << coin << " : $" << std::fixed << std::setprecision(4) << price.price << "\n";
    }

    // Read last row of CSV
    std::ifstream file(csvPath);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }

    if (lines.size() < 2) {
        std::cerr << "\n⚠️ Not enough data in CSV to display risk scores.\n";
        return;
    }

    std::stringstream ss(lines.back());
    std::string token;
    std::vector<std::string> fields;
    while (std::getline(ss, token, ',')) {
        fields.push_back(token);
    }

    if (fields.size() < 8) {
        std::cerr << "\n⚠️ Last row in CSV is malformed.\n";
        return;
    }

    // Extract individual risk fields
    double trend      = std::stod(fields[1]);
    double tweet      = std::stod(fields[2]);
    double liquidity  = std::stod(fields[3]);
    double redemption = std::stod(fields[4]);
    double bridge     = std::stod(fields[5]);
    double whale      = std::stod(fields[6]);
    double uniswap    = std::stod(fields[7]);

    // Risk Breakdown Display
    std::cout << "\n📊  USDC Risk Factors:\n";
    std::cout << "──────────────────────────────────────────────\n";
    std::cout << "🧠 Trend Risk        : " << trend     << "\n";
    std::cout << "🐦 Tweet Risk        : " << tweet     << "\n";
    std::cout << "💧 Liquidity Risk    : " << liquidity << "\n";
    std::cout << "🏦 Redemption Risk   : " << redemption << "\n";
    std::cout << "🌉 Bridge Risk       : " << bridge    << "\n";
    std::cout << "🐋 Whale Risk        : " << whale     << "\n";
    std::cout << "🔄 Uniswap Risk      : " << uniswap   << "\n";

    double finalRiskScore =
    0.25 * trend +
    0.10 * tweet +
    0.20 * liquidity +
    0.10 * redemption +
    0.10 * bridge +
    0.10 * whale +
    0.15 * uniswap;

    // Final Risk Score from main
    std::cout << "\n🚨 USDC Depeg Risk Score = " << std::fixed << std::setprecision(3) << finalRiskScore;
    if (finalRiskScore > 0.7)
        std::cout << " 🔥 High Risk!";
    else if (finalRiskScore > 0.4)
        std::cout << " ⚠️ Moderate Risk";
    else
        std::cout << " ✅ Stable";
    std::cout << "\n──────────────────────────────────────────────\n";
}
