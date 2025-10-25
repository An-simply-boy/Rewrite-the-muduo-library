// EventLoop.cpp
// EventLoop类的实现文件，封装了事件循环。
#include "EventLoop.h"
#include <sys/syscall.h>
#include <iostream>

EventLoop::EventLoop() : ep_(new Epoll), wakefd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(this, wakefd_)), mainloop_(false)
{
    wakechannel_->setreadcallback(bind(&EventLoop::handlewakeup, this)); // 设置读事件回调
    wakechannel_->enablereading(); // 启用读事件
}

EventLoop::~EventLoop()
{
    delete ep_; // 删除epoll对象
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch); // 更新channel的事件
}

void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid); // 获取线程ID
    cout << "事件循环线程ID：" << threadid_ << endl;

    while (true)
    {
        std::vector<Channel *> channels = ep_->loop(); // 等待事件
        for (auto &ch : channels)
        {
            ch->handleevent(); // 处理事件
        }
    }
}

bool EventLoop::isinloopthread()
{
    return (threadid_ == static_cast<pid_t>(syscall(SYS_gettid))); // 判断是否在事件循环线程中
}

void EventLoop::queueinloopthread(function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_); // 加锁
        loopque_.push_back(fn); // 将任务加入队列
    }
    wakeup(); // 唤醒事件循环
}

Epoll *EventLoop::ep()
{
    return ep_; // 返回epoll对象
}

void EventLoop::wakeup()
{
    uint64_t val = 1; // 写入值
    write(wakefd_, &val, sizeof(val)); // 写入wakefd
}

// 处理唤醒事件
void EventLoop::handlewakeup()
{
    uint64_t val;
    read(wakefd_, &val, sizeof(val)); // 读取wakefd
    std::function<void()> fn;
    std::lock_guard<std::mutex> gd(mutex_); // 加锁
    for (auto it = loopque_.begin(); it != loopque_.end();)
    {
        fn = *it;
        fn(); // 执行任务
        loopque_.erase(it++); // 删除任务
        cout << "任务执行完成，剩余队列大小：" << loopque_.size() << endl;
    }
}

void EventLoop::settimeout(bool mainloop, int sec)
{
    mainloop_ = mainloop;
    if (mainloop_)
        return;
    timefd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK); // 创建定时器文件描述符
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 5;
    timerfd_settime(timefd, 0, &timeout, 0); // 设置定时器
    timechannel_ = new Channel(this, timefd);
    timechannel_->setreadcallback(bind(&EventLoop::handletimeout, this, sec)); // 设置读事件回调
    timechannel_->enablereading(); // 启用读事件
}

void EventLoop::handletimeout(int sec)
{
    uint64_t expirations;
    read(timefd, &expirations, sizeof(expirations)); // 读取定时器
    if (!mainloop_)
    {
        for (auto it : conns)
        {
            cout << it.first << " ";
            if (Timestamp::now().toint() - it.second->laststamp() >= sec) // 检查连接是否超时
            {
                conns.erase(it.first); // 删除超时连接
                timeoutcb_(it.second); // 调用超时回调
            }
        }
        cout << endl;
    }
}

void EventLoop::newconnewcton(int fd, spConnection conn)
{
    conns[fd] = conn; // 添加连接
}

void EventLoop::settimeout(function<void(spConnection)> fn)
{
    timeoutcb_ = fn; // 设置超时回调
}