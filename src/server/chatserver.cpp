#include "chatserver.hpp"
#include "json.hpp"
#include <functional>
#include <string>
#include "chatservice.hpp"
#include <iostream>
using json = nlohmann::json;
Chatserver::Chatserver(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&Chatserver::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&Chatserver::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

void Chatserver::start()
{
    _server.start();
}

void Chatserver::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开连接
    if (!conn->connected())
    {
        // 处理客户端异常退出事件
        ChatService::instance()->clientCloseExceptionHandler(conn);
        // 半关闭
        conn->shutdown();
    }
}

void Chatserver::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    // 将json数据流转换为string
    std::string buf = buffer->retrieveAllAsString();    
    // 数据的反序列化
    json js = json::parse(buf);
    // 完全解耦网络模块和业务模块，不要在网络模块中调用业务模块的方法
    // 通过 js["msg_id"] 来获取不同的业务处理器（事先绑定的回调方法）
    // js["msgid"].get<int>() 将js["msgid"]对应的值强制转换成int
    std::cout << "receive json:" << buf << std::endl;
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn, js, time);
}
