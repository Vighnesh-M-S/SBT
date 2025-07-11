#ifndef AAVE_CLIENT_H
#define AAVE_CLIENT_H

#include <string>

class AaveClient {
public:
    void fetchAaveLiquidity();
    double computeLiquidityRisk() const;
    void updateLiquidityScoreCSV(const std::string& csvPath) const;

private:
    double totalAvailableLiquidity_ = 0.0;
};

#endif