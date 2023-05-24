//
// Created by clay on 3/2/23.
//

#include "base/StringUtils.h"
#include "cqBot.h"
#include "base/go-cqHttpAPI/go-cqHttpAPI.h"
#include "muduo/net/EventLoop.h"

int main()
{
    muduo::net::EventLoop loop;
    cqBot bot(&loop, 2050, "Kinoko");

    auto onPrivateMessage = [](const MessageEvent &event, Qid self_id, int64_t time){
        // DEBUG_LOG("private message");
        auto receiver = event.senderId();
        go_cqHttpAPI::send_private_message(receiver, event.messageChain());
    };

    auto onGroupMessage = [&](const MessageEvent &event, Qid self_id, int64_t time){
        // DEBUG_LOG("private message");
        if(!event.messageChain().isAt(self_id)) return ;
        auto group_id = event.rawEvent().at("group_id").get<Qid>();
        std::string msg;
        for(const auto &i : event.messageChain().array()) {
            if(i.type() == MessageBlock::Type::TEXT) {
                auto temp = i.data()["text"].template get<std::string>();
                StringExtra::trim(temp);
                msg.append(temp);
            }
        }
        nlohmann::json m;
        m["data"] = nlohmann::json();
        m["data"]["text"] = msg;
        m["type"] = "text";
        MessageChain c;
        c.array().emplace_back(m);
        
        go_cqHttpAPI::send_group_message(group_id, c);
    };

    bot.setPrivateMessageCallback(onPrivateMessage);
    bot.setGroupMessageCallback(onGroupMessage);

    bot.start();
    loop.loop();

    return 0;
}
