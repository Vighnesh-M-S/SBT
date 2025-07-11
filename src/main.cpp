#include "PriceFeedManager.h"
#include "RiskEngine.h"
#include "HistoricalPriceTracker.h"
#include "EnvLoader.h"
#include "EtherscanClient.h"
#include "UniswapMonitor.h"
#include "AaveClient.h"
#include "CurveClient.h"
#include "TwitterClient.h"
#include "test.h"
#include "BridgeClient.h"
#include "RiskSnapshot.h"
#include <iostream>
#include <thread>
#include <cstdlib> 

int main() {
    EnvLoader::loadEnvFile("../.env");
    // checkApiKey();
    // fetchUniswapStats();
    // AaveClient::fetchAaveLiquidity();
    // CurveClient::fetchCurveTokens();
    TwitterClient::fetchRecentSentiment("usdc depeg");
    // BridgeClient::fetchBridgeEvents();
    // std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};

    // PriceFeedManager manager;
    // manager.start();

    // RiskEngine risk(0.995, 0.990);

    // HistoricalPriceTracker historyTracker;

     
    // fetchUSDCTransfers();   
    // std::vector<RiskSnapshot> allSnapshots;

    // for (const auto& coin : coins) {
    //     auto price = manager.getPrice(coin);
    //     if (price.timestamp == 0) continue;

    //     RiskLevel level = risk.assessRisk(price.price);
    //     auto stats = historyTracker.analyze(coin);
    //     int mentions = TwitterClient::getMentionCount(coin); // assume you stored recent count
    //     double tvl = UniswapClient::getTVL(coin);             // stubbed example
    //     double liquidity = AaveClient::getLiquidity(coin);    // stubbed example
    //     int bridgeEvents = BridgeClient::getUpdates(coin);    // optional

    //     allSnapshots.push_back({
    //         coin,
    //         price.price,
    //         risk.riskToString(level),
    //         stats.trendingDown,
    //         stats.stddev,
    //         mentions,
    //         tvl,
    //         liquidity,
    //         bridgeEvents,
    //         getCurrentTimestamp()
    //     });
    // }

    // saveSnapshotToFile(allSnapshots);

    
    // while (true) {
    //     for (const auto& coin : coins) {
    //         auto price = manager.getPrice(coin);
    //         if (price.timestamp != 0) {
    //             historyTracker.addPrice(coin, price.price);
    //             RiskLevel level = risk.assessRisk(price.price);
    //             auto stats = historyTracker.analyze(coin);
    //             std::cout << coin << ": $" << price.price << " @ " << price.timestamp
    //                       << " → Risk: " << risk.riskToString(level);
    //                       if (stats.trendingDown) std::cout << " , 📉 trending down";
    //                       if (stats.stddev > 0.002) std::cout << " , ⚠️ volatile";
    //                       std::cout << "\n";
    //         } else {
    //             std::cout << coin << ": waiting for valid price...\n";
    //         }
    //     }
    //     std::cout << "----------------------------\n";
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    // }

    return 0;
}