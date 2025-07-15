#include "UniswapMonitor.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <json/json.h>
#include <cstdlib>
#include <iomanip>
#include <algorithm>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

UniswapStats fetchUniswapStats() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ Missing THEGRAPH_API_KEY in environment variables.\n";
        return {};
    }

    std::string url = "https://gateway.thegraph.com/api/subgraphs/id/5zvR82QoaXYFyDEKLZ9t6v9adgnptxYpKpSbxtgVENFV";
    std::string query = R"({
        "query": "query Subgraphs { factories(first: 1) { id poolCount txCount totalVolumeUSD } bundles(first: 1) { id ethPriceUSD } }",
        "operationName": "Subgraphs",
        "variables": {}
      })";

    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        std::string bearer = std::string("Authorization: Bearer ") + apiKey;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, bearer.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return {};
        }
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(response, root)) {
        std::cerr << "❌ Failed to parse JSON response from TheGraph.\n";
        return {};
    }

    try {
        const auto& data = root["data"];
        const auto& factories = data["factories"];
        const auto& bundles = data["bundles"];

        if (!factories.isArray() || factories.empty())
            throw std::runtime_error("No factories found");
        if (!bundles.isArray() || bundles.empty())
            throw std::runtime_error("No bundles found");

        double volume = std::stod(factories[0]["totalVolumeUSD"].asString());
        int pools = std::stoi(factories[0]["poolCount"].asString());
        int txs = std::stoi(factories[0]["txCount"].asString());

        double risk = computeUniswapRiskScore(volume, pools, txs);

        updateUniswapScoreCSV(risk, "/Users/vighneshms/Downloads/SBT/src/model_scores.csv");
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception while parsing TVL fields: " << e.what() << "\n";
    }

    return {};
}

double computeUniswapRiskScore(double volumeUSD, int poolCount, int txCount) {
    const double maxVolume = 2e12;
    const double maxPools = 50000;
    const double maxTx = 1e8;

    double volScore = 1.0 - std::min(volumeUSD / maxVolume, 1.0);
    double poolScore = 1.0 - std::min(poolCount / double(maxPools), 1.0);
    double txScore = 1.0 - std::min(txCount / double(maxTx), 1.0);

    double risk = 0.5 * volScore + 0.3 * poolScore + 0.2 * txScore;
    return risk;
}

void updateUniswapScoreCSV(double score, const std::string& csvPath) {
    std::ifstream in(csvPath);
if (!in) {
    std::cerr << "❌ Failed to open CSV file: " << csvPath << "\n";
    return;
}

std::vector<std::string> lines;
std::string line;
while (std::getline(in, line)) {
    lines.push_back(line);
}
in.close();

if (lines.size() < 2) {
    std::cerr << "⚠️ Not enough rows in CSV to update.\n";
    return;
}

std::stringstream scoreStream;
scoreStream << std::fixed << std::setprecision(6) << score;

std::string& lastLine = lines.back();
std::stringstream ss(lastLine);
std::string item;
std::vector<std::string> cols;

// Split line into columns
while (std::getline(ss, item, ',')) {
    cols.push_back(item);
}

// Ensure at least 8 columns
while (cols.size() < 8) {
    cols.push_back("0.0");
}

// Replace the 8th column (index 7)
cols[7] = scoreStream.str();

// Rebuild the last line
std::ostringstream rebuiltLine;
for (size_t i = 0; i < cols.size(); ++i) {
    if (i > 0) rebuiltLine << ",";
    rebuiltLine << cols[i];
}
lastLine = rebuiltLine.str();

std::ofstream out(csvPath);
if (!out) {
    std::cerr << "❌ Failed to write to CSV file: " << csvPath << "\n";
    return;
}

for (const auto& l : lines) {
    out << l << "\n";
}
out.close();
}