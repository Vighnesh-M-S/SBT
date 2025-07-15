#include "BridgeClient.h"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <fstream>       
#include <sstream>       
#include <vector>        
#include <string>        
#include <algorithm> 

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

static Json::Value lastBridgeData;


void BridgeClient::fetchBridgeEvents() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ Missing THEGRAPH_API_KEY\n";
        return;
    }

    const std::string url = "https://gateway.thegraph.com/api/subgraphs/id/E8ihHMv3ehsjNpopsTAsA8n4tKcNLLhcKUgomRn2cKmF";

    const std::string queryPayload = R"({
        "query": "{ addInboundProofLibraryForChains(first: 5) { id chainId lib blockNumber } appConfigUpdateds(first: 5) { id userApplication configType newConfig } }"
    })";

    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        std::string authHeader = std::string("Authorization: Bearer ") + apiKey;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryPayload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ cURL error: " << curl_easy_strerror(res) << "\n";
            return;
        }

        if (response.find("<!DOCTYPE html>") != std::string::npos) {
            std::cerr << "❌ Received HTML instead of JSON. Check the endpoint or API key.\n";
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(response, root)) {
            std::cerr << "❌ JSON parse error from LayerZero: " << reader.getFormattedErrorMessages() << "\n";
            return;
        }
        BridgeClient::analyzeBridgeEvents(root["data"]);
    }
}

void BridgeClient::analyzeBridgeEvents(const Json::Value& data) {
    const auto& inbound = data["addInboundProofLibraryForChains"];
    const auto& configUpdates = data["appConfigUpdateds"];

    std::unordered_map<int, int> chainEventCount;
    std::unordered_map<std::string, int> configTypeCount;
    std::set<std::string> uniqueUsers;

    // Analyze inbound proof libraries
    for (const auto& entry : inbound) {
        int chainId = entry["chainId"].asInt();
        chainEventCount[chainId]++;
    }

    // Analyze config updates
    for (const auto& entry : configUpdates) {
        std::string configType = entry["configType"].asString();
        std::string userApp = entry["userApplication"].asString();
        configTypeCount[configType]++;
        uniqueUsers.insert(userApp);
    }

    for (const auto& [chainId, count] : chainEventCount) {
    }

    for (const auto& [type, count] : configTypeCount) {
        std::string alert = (count > 2) ? " ⚠️" : "";
    }

    lastBridgeData = data; 
}

double BridgeClient::computeBridgeRisk() {
    if (lastBridgeData.isNull()) {
        std::cerr << "⚠️ [Bridge] No cached data to compute risk.\n";
        return 0.0;
    }

    const auto& inbound = lastBridgeData["addInboundProofLibraryForChains"];
    const auto& updates = lastBridgeData["appConfigUpdateds"];

    int totalEvents = inbound.size() + updates.size();

    // Normalize based on an assumed max of 50 events = high risk
    double risk = std::min(1.0, totalEvents / 50.0);

    return risk;
}

void BridgeClient::updateBridgeScoreCSV(double score, const std::string& csvPath) {
    std::ifstream in(csvPath);
    std::vector<std::string> lines;
    std::string line;

    if (!in) {
        std::cerr << "❌ [Bridge] Cannot open CSV at: " << csvPath << "\n";
        return;
    }

    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lines.size() < 2) {
        std::cerr << "⚠️ Not enough rows in CSV to update.\n";
        return;
    }

    std::stringstream ss(lines.back());
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ',')) fields.push_back(field);

    if (fields.size() < 6) {
        std::cerr << "⚠️ Unexpected CSV format.\n";
        return;
    }

    std::ostringstream scoreStream;
    scoreStream << std::fixed << std::setprecision(6) << score;
    fields[5] = scoreStream.str();  // Update bridgeRisk

    std::ostringstream updatedLine;
    for (size_t i = 0; i < fields.size(); ++i) {
        updatedLine << fields[i];
        if (i < fields.size() - 1) updatedLine << ",";
    }

    lines.back() = updatedLine.str();

    std::ofstream out(csvPath);
    for (const auto& l : lines) out << l << "\n";
}
