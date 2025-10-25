// EchoServer.h
// EchoServer类的头文件，实现了一个简单的回显服务器。
#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

class EchoServer
{
private:
    TcpServer tcpserver_; // TcpServer对象
    ThreadPool threadpool_; // 线程池

public:
    EchoServer(const std::string &ip, const uint16_t port, unsigned short IOthread, unsigned short workthread); // 构造函数
    ~EchoServer(); // 析构函数

    void Start(); // 启动服务器

    void HandleNewConnection(spConnection conn); // 处理新连接
    void HandleClose(spConnection conn); // 处理连接关闭
    void HandleError(spConnection conn); // 处理连接错误
    void HandleMessage(spConnection conn, std::string message); // 处理消息
    void ONMessage(spConnection conn, std::string message); // 消息处理逻辑
    void HandleSendComplete(spConnection conn); // 处理发送完成
};