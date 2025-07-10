cmake_minimum_required(VERSION 3.10)
project(StablecoinTracker)

set(CMAKE_CXX_STANDARD 17)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

add_executable(stablecoin_tracker
    src/main.cpp
    src/PriceFeedManager.cpp
    src/RiskEngine.cpp
    src/HistoricalPriceTracker.cpp
)

target_include_directories(stablecoin_tracker PRIVATE src/include)

target_link_libraries(stablecoin_tracker 
CURL::libcurl 
Threads::Threads
jsoncpp
)