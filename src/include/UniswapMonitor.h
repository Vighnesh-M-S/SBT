#ifndef UNISWAP_MONITOR_H
#define UNISWAP_MONITOR_H

#include <string>

struct UniswapStats {
    double totalVolumeUSD;
    int poolCount;
    int txCount;
};

UniswapStats fetchUniswapStats();
double computeUniswapRiskScore(double volumeUSD, int poolCount, int txCount);
void updateUniswapScoreCSV(double score, const std::string& csvPath);

#endif