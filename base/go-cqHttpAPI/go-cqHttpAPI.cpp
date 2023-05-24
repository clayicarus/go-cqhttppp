#include "go-cqHttpAPI.h"
#include <cpr/cpr.h>

void to_json(nlohmann::json& res, const MessageBlock& mb)
{
    res["data"] = mb.data();
    res["type"] = mb.originType();
}

nlohmann::json go_cqHttpAPI::send_private_message(Qid receiver, const MessageChain &msg)
{
    nlohmann::json req_para;
    req_para["message"] = msg.array();
    req_para["user_id"] = receiver;
    auto r = cpr::Post(cpr::Url(go_cqHttpEndPoint::dump(go_cqHttpEndPoint::SEND_PRIVATE_MSG)),
                        cpr::Header{{"Content-Type", "application/json"}},
                        cpr::Body(req_para.dump()));
    return nlohmann::json::parse(r.text);
}

nlohmann::json go_cqHttpAPI::send_group_message(Qid group_id, const MessageChain &msg) 
{
    nlohmann::json req_para;
    req_para["group_id"] = group_id;
    req_para["message"] = msg.array();
    auto r = cpr::Post(cpr::Url(go_cqHttpEndPoint::HTTP_HOST + std::string(go_cqHttpEndPoint::SEND_GROUP_MSG)),
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Body(req_para.dump()));
    return nlohmann::json::parse(r.text);
}
