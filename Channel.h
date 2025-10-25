// Channel.h
// Channel类的头文件，负责管理单个文件描述符的事件。
#pragma once
#include <sys/epoll.h>
#include "Epoll.h"
#include "Socket.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include <functional>
using namespace std;

class Epoll;
class EventLoop;

class Channel
{
private:
    int fd_ = -1; // 文件描述符
    bool inepoll_ = false; // 是否已在epoll中
    uint32_t events_ = 0; // 需要监视的事件
    uint32_t revents_ = 0; // 已发生的事件
    bool islisten_ = false; // 是否是监听套接字
    std::function<void()> readcallback_; // 读事件回调
    std::function<void()> writecallback_; // 写事件回调
    std::function<void()> closecallback_; // 关闭事件回调
    std::function<void()> errorcallback_; // 错误事件回调
    EventLoop *loop_ = nullptr; // 事件循环

public:
    Channel(EventLoop *loop, int fd); // 构造函数
    ~Channel(); // 析构函数

    int fd(); // 返回文件描述符
    void useet(); // 启用边缘触发
    void enablereading(); // 启用读事件
    void disablereading(); // 禁用读事件
    void enablewriting(); // 启用写事件
    void disablewriting(); // 禁用写事件
    void setinepoll(); // 设置已在epoll中
    void setrevents(uint32_t ev); // 设置已发生的事件
    bool inpoll(); // 返回是否在epoll中
    uint32_t events(); // 返回需要监视的事件
    uint32_t revents(); // 返回已发生的事件
    void handleevent(); // 处理事件

    void setreadcallback(std::function<void()> fn); // 设置读事件回调
    void setwritecallback(std::function<void()> fn); // 设置写事件回调
    void setclosecallback(std::function<void()> fn); // 设置关闭事件回调
    void seterrorcallback(std::function<void()> fn); // 设置错误事件回调
};