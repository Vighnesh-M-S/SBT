#ifndef TWITTER_LOGGER_H
#define TWITTER_LOGGER_H

#include <string>
#include <vector>

class TwitterLogger {
public:
    TwitterLogger(const std::string& filename);
    void logTweets(const std::vector<std::string>& tweets);

private:
    std::string filename_;
};

#endif