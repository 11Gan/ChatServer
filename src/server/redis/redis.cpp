#include "redis.hpp"
#include <iostream>

Redis::Redis() : publish_context_(nullptr), subcribe_context_(nullptr)
{
}

Redis::~Redis()
{
    if (publish_context_ != nullptr)
    {
        redisFree(publish_context_);
    }

    if (subcribe_context_ != nullptr)
    {
        redisFree(subcribe_context_);
    }
}

// 连接Redis服务器
bool Redis::connect()
{
    publish_context_ = redisConnect("127.0.0.1", 6379);
    if (publish_context_ == nullptr)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    subcribe_context_ = redisConnect("127.0.0.1", 6379);
    if (subcribe_context_ == nullptr)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    // 独立线程中接收订阅通道的消息
    thread t([&]()
             { observer_channel_message(); });
    t.detach();

    cout << "connect redis-server success!" << endl;
    return true;
}

// 向Redis指定的通道channel发布消息
bool Redis::publish(int channel, string message)
{
    // 相当于publish 键 值
    // redis 127.0.0.1:6379> PUBLISH CHANNEL "Redis PUBLISH test"
    redisReply *reply = (redisReply *)redisCommand(publish_context_, "PUBLISH %d %s", channel, message.c_str());
    if (reply == nullptr)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }

    // 释放资源
    freeReplyObject(reply);
    return true;
}

// 向Redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel)
{
    // subscribe 命令本身会造成线程阻塞等待通道里面发生消息
    // redisCommand 会先把命令缓存到context中，然后调用RedisAppendCommand发送给redis
    // redis执行subscribe是阻塞，不会响应，不会给我们一个reply
    // redis 127.0.0.1:6379> SUBSCRIBE CHANNEL
    if (REDIS_ERR == redisAppendCommand(subcribe_context_, "SUBSCRIBE %d", channel))
    {
        cerr << "subscibe command failed" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(subcribe_context_, &done))
        {
            cerr << "subscribe command failed" << endl;
            return false;
        }
    }
    cout << "subscribe channel:"<< channel << endl;
    return true;
}

// 取消订阅
bool Redis::unsubscribe(int channel)
{
    // redisCommand 会先把命令缓存到context中，然后调用RedisAppendCommand发送给redis
    if (REDIS_ERR == redisAppendCommand(subcribe_context_, "UNSUBSCRIBE %d", channel))
    {
        cerr << "subscibe command failed" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(subcribe_context_, &done))
        {
            cerr << "subscribe command failed" << endl;
            return false;
        }
    }
    cout << "unsubscribe channel:"<< channel << endl;
    return true;
}


// 独立线程中接收订阅通道的消息
void Redis::observer_channel_message()
{
    while (true)
    {
        // 从订阅通道接收消息
        redisReply *reply;
        int status = redisGetReply(subcribe_context_, (void **)&reply);
        // 检查获取回复消息的状态
        if (status != REDIS_OK)
        {
            cerr << "Failed to receive reply from Redis server." << endl;
            break;;
        }

        // 检查回复消息是否为空
        if (reply == nullptr)
        {
            cerr << "Received NULL reply from Redis server." << endl;
            continue;
        }

        if(reply->type == REDIS_REPLY_ARRAY && reply->elements <3){
            cerr << "Received NULL Array from Redis server." << endl;
            continue;
        }

        // cout << "Reply Type: " << reply->type<<endl;
        // cout << "Reply Elements: " << reply->elements<<endl;
        // 提取通道号和消息内容
        const char *channel_id = reply->element[1]->str;
        const char *message = reply->element[2]->str;

        // 检查通道号和消息内容是否为空
        if (channel_id == nullptr || message == nullptr)
        {
            cerr << "Received invalid channel ID or message from Redis server." << endl;
            freeReplyObject(reply);
            continue;
        }

        // 将消息发送给业务层处理
        cout <<"Channel id:" << channel_id << " MSG:" << message << endl;
        notify_message_handler_(atoi(channel_id), message);
        // 释放回复对象的内存
        freeReplyObject(reply);
    }

    cerr << "----------------------- oberver_channel_message quit--------------------------" << endl;
}


// 初始化业务层上报通道消息的回调对象
void Redis::init_notify_handler(redis_handler handler)
{
    notify_message_handler_ = handler;
}
