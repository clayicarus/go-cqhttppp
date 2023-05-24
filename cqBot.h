#ifndef CQBOT_H
#define CQBOT_H

#include "base/Logger.h"
#include "base/MessageEvent/MessageEvent.h"
#include "base/WebsocketServer/WebsocketServer.h"
#include "base/muduo/net/EventLoop.h"
#include "base/muduo/net/InetAddress.h"
#include <string_view>

class cqBot {
public:
    using MessageEventCallback = std::function<void(const MessageEvent&, Qid, int64_t)>;
    cqBot(muduo::net::EventLoop *loop, uint16_t listen_port, std::string_view bot_name) 
        : ws_server_(loop, muduo::net::InetAddress(listen_port)), 
          bot_name_(bot_name),
          messageCallback([this](const MessageEvent& event, Qid self_id, int64_t time){
            DEBUG_LOG("on message - " + event.rawEvent().dump());
            switch(event.eventType()) {
                case MessageEvent::Type::GROUP_MESSAGE:
                    groupMessageCallback(event, self_id, time);
                    break;
                case MessageEvent::Type::PRIVATE_MESSAGE:
                    privateMessageCallback(event, self_id, time);
                    break;
                default:
                    WARN_LOG("not support message event type");
                    break;
            }
          }),
          groupMessageCallback(defaultMessageEventCallback),
          privateMessageCallback(defaultMessageEventCallback)
    {
        ws_server_.setMessageCallback([this](const auto &conn, const auto &msg, auto time)
                              { onPost(msg); });
    }
    void start() 
    { 
        ws_server_.start(); 
        WARN_LOG(bot_name_ + " start");
    }
    void setGroupMessageCallback(MessageEventCallback cb) { groupMessageCallback = std::move(cb); }
    void setPrivateMessageCallback(MessageEventCallback cb) { privateMessageCallback = std::move(cb); }
private:
    void onPost(const std::string &msg) const;

    void messageSentHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;
    void requestHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;
    void noticeHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;
    void metaEventHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;
    void unknownHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;
    void messageHandler(const nlohmann::json& event, Qid self_id, int64_t time) const;

    static const MessageEventCallback defaultMessageEventCallback;
    MessageEventCallback groupMessageCallback;
    MessageEventCallback privateMessageCallback;
    MessageEventCallback messageCallback;

    WebsocketServer ws_server_;
    const std::string bot_name_;
};

#endif // CQBOT_H