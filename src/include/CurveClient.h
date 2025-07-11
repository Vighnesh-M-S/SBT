#ifndef CURVE_CLIENT_H
#define CURVE_CLIENT_H

#include <string>

class CurveClient {
public:
    static void fetchCurveTokens();
    static double computeCurveRisk();
    static void updateCurveScoreCSV(double score, const std::string& csvPath);
};

#endif
