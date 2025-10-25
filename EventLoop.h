// EventLoop.h
// EventLoop类的头文件，封装了事件循环。
#pragma once
#include "Epoll.h"
#include <queue>
#include <functional>
#include <mutex>
#include <sys/eventfd.h>
#include <time.h>
#include "Channel.h"
#include <sys/timerfd.h>
#include <set>
#include <unordered_map>
#include <map>
#include <memory>
#include "Connection.h"
#include "Timestamp.h"

class Channel;
class Epoll;
class Connection;

using spConnection = std::shared_ptr<Connection>;

struct node
{
    int t;
    int fd;
};

class EventLoop
{
private:
    Epoll *ep_; // epoll对象
    pid_t threadid_; // 线程ID
    std::vector<function<void()>> loopque_; // 任务队列
    int wakefd_; // 唤醒文件描述符
    Channel *wakechannel_; // 唤醒通道
    mutex mutex_; // 互斥锁
    bool mainloop_; // 是否是主线程
    int timefd; // 定时器文件描述符
    Channel *timechannel_; // 定时器通道
    map<int, spConnection> conns; // 连接映射
    function<void(spConnection)> timeoutcb_; // 超时回调

public:
    EventLoop(); // 构造函数
    ~EventLoop(); // 析构函数

    void run(); // 运行事件循环
    void updatechannel(Channel *ch); // 更新channel的事件
    Epoll *ep(); // 返回epoll对象
    bool isinloopthread(); // 判断是否在事件循环线程中
    void queueinloopthread(function<void()>); // 将任务加入队列
    void wakeup(); // 唤醒事件循环
    void handlewakeup(); // 处理唤醒事件
    void settimeout(bool mainloop, int sec); // 设置超时
    void handletimeout(int sec); // 处理超时事件
    void newconnewcton(int fd, spConnection conn); // 添加连接
    void settimeout(function<void(spConnection)> fn); // 设置超时回调
};