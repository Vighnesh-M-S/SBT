#ifndef BRIDGE_CLIENT_H
#define BRIDGE_CLIENT_H

#include <string>
#include <json/json.h>

class BridgeClient {
public:
    static void fetchBridgeEvents();
    static void analyzeBridgeEvents(const Json::Value& data); 
};

#endif
