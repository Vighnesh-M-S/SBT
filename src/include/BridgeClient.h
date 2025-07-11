#ifndef BRIDGE_CLIENT_H
#define BRIDGE_CLIENT_H

#include <string>
#include <json/json.h>

class BridgeClient {
public:
    static void fetchBridgeEvents();
    static void analyzeBridgeEvents(const Json::Value& data);
    static double computeBridgeRisk();
    static void updateBridgeScoreCSV(double score, const std::string& csvPath);
};

#endif