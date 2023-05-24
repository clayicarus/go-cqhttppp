project(go-cqhttppp)
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

set(UNOFFICIAL_LINK_LIST
        muduo_net
        muduo_base
        pthread
        nlohmann_json::nlohmann_json
        cpr::cpr
        unofficial::sqlite3::sqlite3
        )
set(SOURCE_LIST
        base/WebsocketServer/WebsocketServer.cpp
        base/WebsocketServer/DataFrameCodec.cpp

        base/HttpParser.cpp
        base/base64.cpp
        base/sha1_portable.cpp
        base/Logger.cpp

        base/MessageEvent/MessageEvent.cpp
        base/MessageEvent/MessageChain.cpp
        base/go-cqHttpAPI/go-cqHttpAPI.cpp
        
        cqBot.cpp)

include_directories(base)
link_directories(lib)

add_executable(${PROJECT_NAME} main.cpp ${SOURCE_LIST})
target_link_libraries(${PROJECT_NAME} PRIVATE ${UNOFFICIAL_LINK_LIST})

add_executable(test test.cpp ${SOURCE_LIST})
target_link_libraries(test PRIVATE ${UNOFFICIAL_LINK_LIST})
