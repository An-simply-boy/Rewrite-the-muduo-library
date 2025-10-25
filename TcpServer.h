// TcpServer.h
// TcpServer类的头文件，封装了TCP服务器的逻辑。
#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.hpp"
#include <map>
#include <memory>
#include <mutex>

class Acceptor;

class TcpServer
{
private:
    EventLoop loop_; // 事件循环
    Acceptor *acceptor_; // Acceptor对象
    ThreadPool pool_; // 线程池
    mutex mt; // 互斥锁
    map<int, spConnection> conns; // 连接映射
    int cnt = 0; // 计数器

    std::vector<unique_ptr<EventLoop>> subloop_; // 子事件循环

    std::function<void(spConnection)> newconnectioncb_; // 新连接回调
    std::function<void(spConnection)> closeconnectioncb_; // 关闭回调
    std::function<void(spConnection)> errorconnectioncb_; // 错误回调
    std::function<void(spConnection, std::string &message)> onmessagecb_; // 消息回调
    std::function<void(spConnection)> sendcompletecb_; // 发送完成回调
    std::function<void(EventLoop *)> timeoutcb_; // 超时回调

    void handeltimeout(spConnection conn); // 处理超时

public:
    TcpServer(const std::string &ip, const uint16_t port, unsigned short threadnum); // 构造函数
    ~TcpServer(); // 析构函数

    void newconnection(Socket *clientsock); // 处理新连接
    void closeconnection(spConnection conn); // 处理关闭连接
    void errorconnection(spConnection conn); // 处理错误连接
    void onmessage(spConnection conn, string message); // 处理消息

    void start(); // 启动服务器

    void setnewconnectioncb(std::function<void(spConnection)> fn); // 设置新连接回调
    void setcloseconnectioncb(std::function<void(spConnection)> fn); // 设置关闭回调
    void seterrorconnectioncb(std::function<void(spConnection)> fn); // 设置错误回调
    void setonmessagecb(std::function<void(spConnection, std::string &message)> fn); // 设置消息回调
    void setsendcompletecb(std::function<void(spConnection)> fn); // 设置发送完成回调
    void settimeoutcb(std::function<void(EventLoop *)> fn); // 设置超时回调
};