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
#include <chrono>

int main() {
    EnvLoader::loadEnvFile("../.env");
    // checkApiKey();
    // fetchUniswapStats();
    // AaveClient::fetchAaveLiquidity();
    // CurveClient::fetchCurveTokens();
    TwitterClient client;
    // BridgeClient::fetchBridgeEvents();
    // std::vector<std::string> coins = {"usdc", "usdt", "dai", "frax", "busd"};

    // PriceFeedManager manager;
    // manager.start();
    const std::string keyword = "usdc depeg";
    const std::string tweetFile = "usdc_tweets.txt";

    while (true) {
        // clearTweetFile(tweetFile);
        client.fetchRecentSentiment(keyword);

        std::cout << "⏳ Waiting 1 minute before next fetch...\n";
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }


    // RiskEngine risk(0.995, 0.990);

    // HistoricalPriceTracker tracker(60); 

     
    // fetchUSDCTransfers();   
    // std::vector<RiskSnapshot> allSnapshots;

    
        // HistoricalPriceTracker tracker(6);  // Track last 10 minutes (60 x 10s)
        // std::string coin = "usdc";
    
        // Initial warmup: collect 10 mins worth of data
        
        // while (true) {
        //     for (int i = 0; i < 6; ++i) {
        //         auto price = manager.getPrice(coin);
        //         if (price.timestamp != 0) {
        //             tracker.addPrice(coin, price.price);
        //         }
        //         std::this_thread::sleep_for(std::chrono::seconds(10));
        //     }
    
        //     tracker.updateRiskCSV(coin, "model_scores.csv");
        //     std::cout << "🔄 Updated trend score\n";
        // }
    
        return 0;
    }
    