// Connection.h
// Connection类的头文件，负责管理单个客户端连接。
#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include "Timestamp.h"

class Connection;
class EventLoop;
class Channel;

using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_; // 事件循环
    Socket *clientsock_; // 客户端套接字
    std::unique_ptr<Channel> clientchannel_; // 客户端通道
    std::function<void(spConnection)> closecallback_; // 关闭事件回调
    std::function<void(spConnection)> errorcallback_; // 错误事件回调
    std::function<void(spConnection, string &)> onmessagecallback_; // 消息处理回调
    Buffer inputbuffer_; // 输入缓冲区
    Buffer outputbuffer_; // 输出缓冲区
    Timestamp ts; // 时间戳

public:
    Connection(EventLoop *loop, Socket *clientsock); // 构造函数
    ~Connection(); // 析构函数

    string ip() const; // 返回客户端IP
    uint16_t port() const; // 返回客户端端口
    int fd() const; // 返回文件描述符

    void closecallback(); // 处理关闭事件
    void errorcallback(); // 处理错误事件
    void writecallback(); // 处理写事件
    void setclosecallback(std::function<void(spConnection)> fn); // 设置关闭事件回调
    void seterrorcallback(std::function<void(spConnection)> fn); // 设置错误事件回调
    void setonmessagecallback(std::function<void(spConnection, string)> fn); // 设置消息处理回调
    void onmessage(); // 处理读事件
    void send(const char *data, size_t size); // 发送数据
    void sendinloop(const char *data, size_t size); // 在事件循环线程中发送数据
    int laststamp(); // 返回时间戳
};