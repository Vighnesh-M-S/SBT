#include "CurveClient.h"
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>
#include <cstdlib>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void CurveClient::fetchCurveTokens() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ THEGRAPH_API_KEY not set in environment." << std::endl;
        return;
    }

    std::string url = "https://gateway.thegraph.com/api/subgraphs/id/3fy93eAT56UJsRCEht8iFhfi6wjHWXtZ9dnnbQmvFopF";

    std::string query = R"({
        tokens(first: 5) {
            id
            name
            symbol
            decimals
        }
        rewardTokens(first: 5) {
            id
            token {
                id
            }
            type
        }
    })";

    Json::Value jsonRequest;
    jsonRequest["query"] = query;
    jsonRequest["operationName"] = "Subgraphs";
    jsonRequest["variables"] = Json::objectValue;

    Json::StreamWriterBuilder writer;
    std::string requestBody = Json::writeString(writer, jsonRequest);

    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string authHeader = "Authorization: Bearer ";
        authHeader += apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return;
        }

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream s(response);
        if (!Json::parseFromStream(reader, s, &root, &errs)) {
            std::cerr << "❌ Failed to parse JSON response from TheGraph: " << errs << std::endl;
            return;
        }

        if (root.isMember("errors")) {
            std::cerr << "❌ Error in GraphQL response: " << root["errors"] << std::endl;
            return;
        }

        std::cout << "✅ Curve Tokens Response Parsed:\n" << root["data"] << std::endl;
    }
}