//
// Created by clay on 3/2/23.
//

// #include "base/WebsocketServer/WebsocketServer.h"
// #include "muduo/net/EventLoop.h"
// #include "base/Logger.h"
// #include "base/StringUtils.h"
// #include "base/MessageEvent/MessageEvent.h"
// #include "base/go-cqHttpAPI/go-cqHttpAPI.h"

// #include <cpr/cpr.h>
// #include <nlohmann/json.hpp>
// #include <map>

// #include "base/BotConstant.h"

// using MessageEventCallback = std::function<void(const MessageEvent&, Qid, int64_t)>;
// MessageEventCallback onMessage;

// static constexpr char BOT_NAME[] = "Kinoko";

// void messageSentHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("messageSentHandler - " + event.dump());
// }
// void messageHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("messageHandler - " + event.dump());
//     for(const auto * i : message_event_paralist) {
//         if(!event.contains(i)) {
//             WARN_LOG("messageHandler - invalid event without " + std::string(i));
//             return;
//         }
//     }
//     onMessage(MessageEvent(event), self_id, time);
// }
// void requestHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("requestHandler - " + event.dump());
// }
// void noticeHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("noticeHandler - " + event.dump());
// }
// void metaEventHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("metaEventHandler - " + event.dump());
// }
// void unknownHandler(const nlohmann::json& event, Qid self_id, int64_t time)
// {
//     DEBUG_LOG("unknownHandler - " + event.dump());
// }

// void onPost(const std::string &msg)
// {
//     auto event = nlohmann::json::parse(msg);
//     if(!event.contains("post_type") || !event.contains("self_id") || !event.contains("time")) {
//         WARN_LOG("invalid go-cqhttp post");
//         return;
//     }
//     std::string post_type = event["post_type"];
//     int64_t time = event["time"];
//     Qid self_id = event["self_id"];
//     // TODO: pack time
//     typedef std::function<void(const nlohmann::json&, Qid, int64_t)> PostHandler;
//     const std::map <std::string, PostHandler> handler_map = {
//             {"message", messageHandler},
//             {"message_sent", messageSentHandler},
//             {"request", requestHandler},
//             {"notice", noticeHandler},
//             {"meta_event", metaEventHandler}
//     };
//     event.erase("post_type");
//     event.erase("time");
//     event.erase("self_id");
//     auto p = handler_map.find(post_type);
//     if(p != handler_map.end()) {
//         p->second(event, self_id, time);
//     } else {
//         unknownHandler(event, self_id, time);
//     }
// }

// // void to_json(nlohmann::json& res, const MessageBlock& mb)
// // {
// //     res["data"] = mb.data();
// //     res["type"] = mb.originType();
// // }

// int main()
// {
//     onMessage = [&](const auto &event, Qid self_id, auto time) {
//         DEBUG_LOG("on message - " + event.rawEvent().dump());
//         auto onGroupMessage = [&](){
//             if(!event.messageChain().isAt(self_id)) return ;
//             nlohmann::json req_para;
//             auto group_id = event.rawEvent().at("group_id").template get<Qid>();
//             req_para["group_id"] = group_id;
//             std::string msg;
//             for(const auto &i : event.messageChain().array()) {
//                 if(i.type() == MessageBlock::Type::TEXT) {
//                     auto temp = i.data()["text"].template get<std::string>();
//                     StringExtra::trim(temp);
//                     msg.append(temp);
//                 }
//             }

//         };
//         auto onPrivateMessage = [&](){
//             nlohmann::json req_para;
//             // repeat bot
//             req_para["message"] = event.messageChain().array();
//             req_para["user_id"] = event.senderId();
//             auto r = cpr::Post(cpr::Url(go_cqHttpEndPoint::dump(go_cqHttpEndPoint::SEND_PRIVATE_MSG)),
//                                cpr::Header{{"Content-Type", "application/json"}},
//                                cpr::Body(req_para.dump()));
//             DEBUG_LOG(r.text);
//         };

//         switch(event.eventType()) {
//             case MessageEvent::Type::GROUP_MESSAGE:
//                 DEBUG_LOG("group message");
//                 onGroupMessage();
//                 break;
//             case MessageEvent::Type::PRIVATE_MESSAGE:
//                 DEBUG_LOG("private message");
//                 onPrivateMessage();
//                 break;
//             default:
//                 WARN_LOG("not support message event type");
//                 break;
//         }
//     };

//     muduo::Logger::setLogLevel(muduo::Logger::INFO);
//     muduo::net::EventLoop loop;
//     WebsocketServer server(&loop, muduo::net::InetAddress(2050));
//     server.setMessageCallback([](const auto &conn, const auto &msg, auto time)
//                               { onPost(msg); });

//     server.start();
//     loop.loop();

//     return 0;
// }

#include "base/Logger.h"
#include "base/StringUtils.h"
#include "cqBot.h"
#include "base/go-cqHttpAPI/go-cqHttpAPI.h"
#include "muduo/net/EventLoop.h"
#include <sqlite3.h>
#include <string>
using namespace std;

constexpr char DB_NAME[] = "../messages.db";

string dump_as_insert(int64_t group_id, int64_t sender_id, int64_t msg_time, string_view context)
{
    static const string tb_name = "messages";
    static const vector<string> cols = {
        "id", "group_id", "sender_id", "msg_time", "context"
    };
    string insert_cmd("insert into " + tb_name);
    string temp("(");
    for(const auto &i : cols) {
        temp += i;
        temp.push_back(',');
    }

    if(temp.size() > 1) 
        temp.pop_back();
    temp += ")";
    insert_cmd.append(temp + "values(null,");
    insert_cmd.append(to_string(group_id));
    insert_cmd.push_back(',');
    insert_cmd.append(to_string(sender_id));
    insert_cmd.push_back(',');
    insert_cmd.append(to_string(msg_time));
    insert_cmd.push_back(',');
    string c(context);
    insert_cmd.append('\'' + c + "\');");    // if null

    return insert_cmd;
}

int main()
{
    muduo::net::EventLoop loop;
    cqBot bot(&loop, 2050, "Kinoko");
    sqlite3 *db;    // close ?
    char *zErrMsg = NULL;
    int rc;
    rc = sqlite3_open(DB_NAME, &db);
    if(rc) {
        FATAL_LOG(std::string("sqlite3 - Can't open database: ") + sqlite3_errmsg(db));
    } 
    INFO_LOG("sqlite3 - Opened database successfully");

    auto onPrivateMessage = [](const MessageEvent &event, Qid self_id, int64_t time){
        INFO_LOG(" - onPrivateMessage " + event.rawEvent().dump());
        auto receiver = event.senderId();
        go_cqHttpAPI::send_private_message(receiver, event.messageChain());
    };

    auto onGroupMessage = [&](const MessageEvent &event, Qid self_id, int64_t time){
        INFO_LOG(" - onGroupMessage " + event.rawEvent().dump());
        Qid group_id = event.rawEvent().at("group_id").get<Qid>();
        std::string msg;
        for(const auto &i : event.messageChain().array()) {
            if(i.type() == MessageBlock::Type::TEXT) {
                auto temp = i.data()["text"].template get<std::string>();
                StringExtra::trim(temp);
                msg.append(temp);
            }
        }
        if(msg.empty()) {
            INFO_LOG("onGroupMessage - recv empty msg");
            return;
        }
        auto insert_cmd = dump_as_insert(group_id, event.senderId(), time, msg);
        rc = sqlite3_exec(db, insert_cmd.c_str(), NULL, NULL, &zErrMsg);
        if(rc != SQLITE_OK) {
            ERROR_LOG(string("sqlite3 - \"") + insert_cmd + "\": " + zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            DEBUG_LOG("sqlite3 - insert successfully");
        }
    };

    bot.setPrivateMessageCallback(onPrivateMessage);
    bot.setGroupMessageCallback(onGroupMessage);

    bot.start();
    loop.loop();

    return 0;
}
