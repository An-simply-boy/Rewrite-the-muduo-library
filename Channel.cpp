// Channel.cpp
// Channel类的实现文件，负责管理单个文件描述符的事件。
#include "Channel.h"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd)
{
    // 构造函数
}

Channel::~Channel()
{
    // 析构函数
}

int Channel::fd()
{
    return fd_; // 返回文件描述符
}

void Channel::useet()
{
    events_ = events_ | EPOLLET; // 启用边缘触发
}

void Channel::enablereading()
{
    events_ |= EPOLLIN; // 启用读事件
    loop_->ep()->updatechannel(this); // 更新epoll
}

void Channel::enablewriting()
{
    events_ |= EPOLLOUT; // 启用写事件
    loop_->ep()->updatechannel(this); // 更新epoll
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN; // 禁用读事件
    loop_->ep()->updatechannel(this); // 更新epoll
}

void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT; // 禁用写事件
    loop_->ep()->updatechannel(this); // 更新epoll
}

void Channel::setinepoll()
{
    inepoll_ = true; // 设置已在epoll中
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev; // 设置已发生的事件
}

bool Channel::inpoll()
{
    return inepoll_; // 返回是否在epoll中
}

uint32_t Channel::events()
{
    return events_; // 返回需要监视的事件
}

uint32_t Channel::revents()
{
    return revents_; // 返回已发生的事件
}

void Channel::handleevent()
{
    if (revents_ & EPOLLRDHUP)
    {
        closecallback_(); // 处理连接关闭事件
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        readcallback_(); // 处理读事件
    }
    else if (revents_ & EPOLLOUT)
    {
        writecallback_(); // 处理写事件
    }
    else
    {
        errorcallback_(); // 处理错误事件
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn; // 设置读事件回调
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_ = fn; // 设置写事件回调
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn; // 设置关闭事件回调
}

void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn; // 设置错误事件回调
}