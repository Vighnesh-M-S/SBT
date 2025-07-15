#include "TwitterLogger.h"
#include <fstream>
#include <iostream>

TwitterLogger::TwitterLogger(const std::string& filename)
    : filename_(filename) {}

void TwitterLogger::logTweets(const std::vector<std::string>& tweets) {
    // Open in truncate mode to overwrite file every time
    std::ofstream file("/Users/vighneshms/Downloads/SBT/src/" + filename_, std::ios::trunc);
    if (!file) {
        std::cerr << "❌ Failed to open " << filename_ << " for writing tweets.\n";
        return;
    }

    for (const auto& tweet : tweets) {
        file << tweet << "\n";
    }
}