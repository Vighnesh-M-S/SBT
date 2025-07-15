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
    
    TwitterClient client;
    

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
        
        

        for (int i = 0; i < 6; ++i) {
                    auto price = manager.getPrice(coin);
                    
                    if (price.timestamp != 0) {
                        tracker.addPrice(coin, price.price);
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                }
        
                tracker.updateRiskCSV(coin, loc);
                
        client.fetchRecentSentiment(keyword);
        int result = std::system("python3 /Users/vighneshms/Downloads/SBT/src/tweet_score_updater.py");


        AaveClient client2;
        client2.fetchAaveLiquidity();
        client2.updateLiquidityScoreCSV("/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
        CurveClient::fetchCurveTokens();
        double score = CurveClient::computeCurveRisk();
        CurveClient::updateCurveScoreCSV(score, "/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
        
        BridgeClient::fetchBridgeEvents();
        double score2 = BridgeClient::computeBridgeRisk();
        BridgeClient::updateBridgeScoreCSV(score2, "/Users/vighneshms/Downloads/SBT/src/model_scores.csv");

        fetchUSDCTransfers();
        UniswapStats stats = fetchUniswapStats();
        if (stats.totalVolumeUSD > 0 && stats.poolCount > 0 && stats.txCount > 0) {
            double score3 = computeUniswapRiskScore(stats.totalVolumeUSD, stats.poolCount, stats.txCount);
            updateUniswapScoreCSV(score3, loc);
        }

    
        
        std::this_thread::sleep_for(std::chrono::minutes(1));

        
        }
        
        return 0;
    }
    