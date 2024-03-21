#include <muduo/base/Logging.h>
#include <iostream>
#include <signal.h>

#include "chatserver.hpp"
#include "chatservice.hpp"
// 捕获SIGINT的处理函数
void resetHandler(int)
{
    LOG_INFO << "capture the SIGINT, will reset state\n";
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << std::endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    const char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    Chatserver server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}