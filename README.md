## go-cqhttppp

适用于 [go-cqhttp](https://github.com/Mrs4s/go-cqhttp) 的轻量 C++ SDK。目前仅支持 Linux。

### 编译环境

- ubuntu 22.04

### 编译前的准备

- 依赖 muduo 网络库
- 安装vcpkg并导入如下的第三方库
  - HTTP 库： [C++ Requests](https://github.com/libcpr/cpr)
  - JSON 库：[JSON for Modern C++](https://github.com/nlohmann/json)

### 复读机器人 Demo

```cpp

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

    bot.setPrivateMessageCallback(onPrivateMessage);

    bot.start();
    loop.loop();

    return 0;
}

```
