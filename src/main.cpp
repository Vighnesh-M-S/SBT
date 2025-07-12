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
#include "DepegPredictor.h"
#include "Dashboard.h"
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

    RiskEngine risk(0.995, 0.990);

    HistoricalPriceTracker tracker(60); 

    PriceFeedManager manager;
    manager.start();
    const std::string keyword = "usdc depeg";
    const std::string tweetFile = "usdc_tweets.txt";
    const std::string loc = "/Users/vighneshms/Downloads/SBT/src/model_scores.csv";
    const std::string coin = "usdc";
    double finalScore = 0.0;

    while (true) {
        showDashboard(loc, manager);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    while (true) {
        // clearTweetFile(tweetFile);
        // showDashboard(loc, manager, finalScore);
        

        for (int i = 0; i < 6; ++i) {
                    auto price = manager.getPrice(coin);
                    // std::cout << "💱 " << coin << " price: " << price.price << "\n";
                    if (price.timestamp != 0) {
                        tracker.addPrice(coin, price.price);
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                }
        
                tracker.updateRiskCSV(coin, loc);
                // std::cout << "🔄 Updated trend score\n";
        client.fetchRecentSentiment(keyword);
        int result = std::system("python3 /Users/vighneshms/Downloads/SBT/src/tweet_score_updater.py");
    // if (result != 0) {
    //     std::cerr << "❌ Python script failed.\n";
    // } else {
    //     std::cout << "✅ Python tweet scoring script completed.\n";
    // }

        AaveClient client2;
        client2.fetchAaveLiquidity();
        client2.updateLiquidityScoreCSV("/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
        CurveClient::fetchCurveTokens();
        double score = CurveClient::computeCurveRisk();
        CurveClient::updateCurveScoreCSV(score, "/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
        // std::this_thread::sleep_for(std::chrono::minutes(1));
        BridgeClient::fetchBridgeEvents();
        double score2 = BridgeClient::computeBridgeRisk();
        BridgeClient::updateBridgeScoreCSV(score2, "/Users/vighneshms/Downloads/SBT/src/model_scores.csv");

        fetchUSDCTransfers();
        UniswapStats stats = fetchUniswapStats();
        if (stats.totalVolumeUSD > 0 && stats.poolCount > 0 && stats.txCount > 0) {
            double score3 = computeUniswapRiskScore(stats.totalVolumeUSD, stats.poolCount, stats.txCount);
            updateUniswapScoreCSV(score3, loc);
        }

    
        // double finalScore = computeDepegRiskScore("/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
        std::this_thread::sleep_for(std::chrono::minutes(1));

        
        }
        

    

     
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
    