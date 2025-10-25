// Epoll.cpp
// Epoll类的实现文件，封装了epoll的使用。
#include "Epoll.h"

Epoll::Epoll()
{
    if ((epollfd_ = epoll_create(1)) == -1) // 创建epoll句柄
    {
        printf("epoll_create()失败(%d).\n", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_); // 关闭epoll句柄
}

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();
    if (ch->inpoll()) // 如果channel已在epoll中
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1) // 修改事件
        {
            printf("epoll_ctl()修改失败(%d).\n", errno);
            exit(-1);
        }
    }
    else
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1) // 添加事件
        {
            printf("epoll_ctl()添加失败(%d).\n", errno);
            exit(-1);
        }
        ch->setinepoll();
    }
}

std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels; // 存放发生事件的channel
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout); // 等待事件
    if (infds < 0)
    {
        perror("epoll_wait()失败");
        exit(-1);
    }
    if (infds == 0)
    {
        printf("epoll_wait()超时。\n");
        return channels;
    }
    for (int ii = 0; ii < infds; ii++) // 遍历发生事件的channel
    {
        Channel *ch = (Channel *)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }
    return channels;
}