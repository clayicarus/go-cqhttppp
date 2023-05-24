#ifndef GO_CQHTTPAPI_H
#define GO_CQHTTPAPI_H

#include "base/MessageEvent/MessageChain.h"
#include "base/TypeDefine.h"
#include "go-cqHttpEndPoint.h"

void to_json(nlohmann::json& res, const MessageBlock& mb);

struct go_cqHttpAPI {
    static nlohmann::json send_private_message(Qid receiver, const MessageChain &msg);
    static nlohmann::json send_group_message(Qid group_id, const MessageChain &msg);
};


#endif