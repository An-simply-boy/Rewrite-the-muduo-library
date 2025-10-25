// Epoll.h
// Epoll类的头文件，封装了epoll的使用。
#pragma once
#include <sys/epoll.h>
#include "Channel.h"
#include <vector>
#include <unistd.h>

class Channel;

class Epoll
{
private:
    static const int MaxEvents = 100; // epoll_wait()返回事件数组的最大大小
    int epollfd_; // epoll句柄
    epoll_event events_[MaxEvents]; // 存放epoll_wait()返回的事件

public:
    Epoll(); // 构造函数
    ~Epoll(); // 析构函数

    void updatechannel(Channel *ch); // 更新channel的事件
    std::vector<Channel *> loop(int timeout = -1); // 等待事件
};