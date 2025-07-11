#ifndef TWITTER_CLIENT_H
#define TWITTER_CLIENT_H
#include <string>


class TwitterClient {
public:
    
    static void fetchRecentSentiment(const std::string& keyword);
   
};

// private:
//     std::string filename_;

#endif