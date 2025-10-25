// Acceptor.h
// Acceptor类的头文件，负责监听新连接。
#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
class TcpServer; // 前向声明

class Acceptor
{
private:
    EventLoop *loop_; // 事件循环
    Socket servsock_; // 监听套接字
    Channel acceptchannel_; // 监听通道
    std::function<void(Socket *)> newconnectioncb_; // 新连接回调函数

public:
    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port); // 构造函数
    ~Acceptor(); // 析构函数

    void setnewconnectioncb(std::function<void(Socket *)> fn); // 设置新连接回调
    void newconnection(); // 处理新连接
};