cmake_minimum_required(VERSION 3.10)
project(StablecoinTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)


include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

add_executable(stablecoin_tracker
    src/main.cpp
    src/PriceFeedManager.cpp
    src/RiskEngine.cpp
    src/HistoricalPriceTracker.cpp
    src/EtherscanClient.cpp
    src/EnvLoader.cpp
    src/test.cpp
    src/UniswapMonitor.cpp
    src/AaveClient.cpp
    src/CurveClient.cpp
    src/TwitterClient.cpp
    src/BridgeClient.cpp
    src/RiskLogger.cpp
    src/TwitterLogger.cpp
    src/DepegPredictor.cpp
    src/Dashboard.cpp
)

target_include_directories(stablecoin_tracker PRIVATE src/include)

target_link_libraries(stablecoin_tracker 
CURL::libcurl 
Threads::Threads
jsoncpp
)