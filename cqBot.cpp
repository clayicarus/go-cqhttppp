#include "base/Logger.h"
#include "base/StringUtils.h"
#include "base/MessageEvent/MessageEvent.h"
#include "base/BotConstant.h"
#include "cqBot.h"
#include <nlohmann/json.hpp>

const cqBot::MessageEventCallback cqBot::defaultMessageEventCallback = [](const MessageEvent&, Qid, int64_t) {
    DEBUG_LOG("defaut event callback");
};

void cqBot::onPost(const std::string &msg) const
{
    auto event = nlohmann::json::parse(msg);
    if(!event.contains("post_type") || !event.contains("self_id") || !event.contains("time")) {
        WARN_LOG("invalid go-cqhttp post");
        return;
    }
    std::string post_type = event["post_type"];
    int64_t time = event["time"];
    Qid self_id = event["self_id"];
    // TODO: pack time into messageEvent
    typedef std::function<void(const nlohmann::json&, Qid, int64_t)> PostHandler;
    auto message = [this](const nlohmann::json& event, Qid self_id, int64_t time) { 
        messageHandler(event, self_id, time); 
    };
    auto message_sent = [this](const nlohmann::json& event, Qid self_id, int64_t time) { 
        messageSentHandler(event, self_id, time); 
    };
    auto request = [this](const nlohmann::json& event, Qid self_id, int64_t time) { 
        requestHandler(event, self_id, time); 
    };
    auto notice = [this](const nlohmann::json& event, Qid self_id, int64_t time) { 
        noticeHandler(event, self_id, time); 
    };
    auto meta_event = [this](const nlohmann::json& event, Qid self_id, int64_t time) { 
        metaEventHandler(event, self_id, time); 
    };
    const std::map <std::string, PostHandler> handler_map = {
            {"message", message },
            {"message_sent", message_sent },
            {"request", request },
            {"notice", notice },
            {"meta_event", meta_event }
    };
    event.erase("post_type");
    event.erase("time");
    event.erase("self_id");
    auto p = handler_map.find(post_type);
    if(p != handler_map.end()) {
        p->second(event, self_id, time);
    } else {
        unknownHandler(event, self_id, time);
    }
}

void cqBot::messageSentHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("messageSentHandler - " + event.dump());
}

void cqBot::requestHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("requestHandler - " + event.dump());
}
void cqBot::noticeHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("noticeHandler - " + event.dump());
}
void cqBot::metaEventHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("metaEventHandler - " + event.dump());
}
void cqBot::unknownHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("unknownHandler - " + event.dump());
}

void cqBot::messageHandler(const nlohmann::json& event, Qid self_id, int64_t time) const
{
    INFO_LOG("messageHandler - message recv");
    for(const auto * i : message_event_paralist) {
        if(!event.contains(i)) {
            WARN_LOG("messageHandler - invalid event without " + std::string(i));
            return;
        }
    }
    messageCallback(MessageEvent(event), self_id, time);
}
