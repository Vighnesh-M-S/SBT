#include "TwitterLogger.h"
#include <fstream>
#include <iostream>

TwitterLogger::TwitterLogger(const std::string& filename)
    : filename_(filename) {}

void TwitterLogger::logTweets(const std::vector<std::string>& tweets) {
    std::ofstream file(filename_, std::ios::app);  // append mode
    if (!file) {
        std::cerr << "❌ Failed to open " << filename_ << " for writing tweets.\n";
        return;
    }

    for (const auto& tweet : tweets) {
        file << tweet << "\n";
    }

    std::cout << "✅ Logged " << tweets.size() << " tweets to " << filename_ << "\n";
}

