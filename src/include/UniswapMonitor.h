#pragma once
#include <string>

struct UniswapStats {
    int poolCount;
    long txCount;
    double totalVolumeUSD;
    double ethPriceUSD;
    bool valid = false;
};

UniswapStats fetchUniswapStats();