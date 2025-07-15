#include "TwitterClient.h"
#include "TwitterLogger.h"
#include <curl/curl.h>
#include <json/json.h>
#include <iostream>
#include <cstdlib>
#include <vector>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total = size * nmemb;
    output->append((char*)contents, total);
    return total;
}

void TwitterClient::fetchRecentSentiment(const std::string& keyword) {
    const char* bearer = std::getenv("TWITTER_BEARER_TOKEN");
    if (!bearer) {
        std::cerr << "❌ Twitter API key not found\n";
        return;
    }

    std::string encodedKeyword = curl_easy_escape(nullptr, keyword.c_str(), keyword.length());
    std::string url = "https://api.twitter.com/2/tweets/search/recent?query=" + encodedKeyword + "&max_results=10";

    std::string response;
    CURL* curl = curl_easy_init();
    struct curl_slist* headers = nullptr;
    std::vector<std::string> tweetTexts;

    if (curl) {
        std::string auth = "Authorization: Bearer " + std::string(bearer);
        headers = curl_slist_append(headers, auth.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << "\n";
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (reader.parse(response, root)) {
            const auto& tweets = root["data"];
            if (!tweets || tweets.empty()) {
                return;
            }

            for (const auto& tweet : tweets) {
                std::string text = tweet["text"].asString();
                tweetTexts.push_back(text);
            }

            // Write fresh batch (overwrites previous)
            TwitterLogger logger("usdc_tweets.txt");
            logger.logTweets(tweetTexts);

        } else {
            std::cerr << "❌ Failed to parse Twitter response\n";
        }
    }
}
