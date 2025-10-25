// Socket.cpp
// Socket类的实现文件，封装了套接字操作。
#include "Socket.h"

int createnonblocking()
{
    // 创建非阻塞套接字
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd)
{
    // 构造函数
}

Socket::~Socket()
{
    ::close(fd_); // 关闭套接字
}

int Socket::fd() const
{
    return fd_; // 返回文件描述符
}

void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)); // 设置TCP_NODELAY选项
}

void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // 设置SO_REUSEADDR选项
}

void Socket::setreuseport(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)); // 设置SO_REUSEPORT选项
}

void Socket::setkeepalive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); // 设置SO_KEEPALIVE选项
}

void Socket::bind(const InetAddress &servaddr)
{
    ::bind(fd_, servaddr.addr(), sizeof(sockaddr)); // 绑定地址
    setipport(servaddr.ip(), servaddr.port()); // 设置IP和端口
}

void Socket::listen(int nn)
{
    ::listen(fd_, nn); // 开始监听
}

int Socket::accept(InetAddress &clientaddr)
{
    socklen_t len = sizeof(clientaddr);
    return accept4(fd_, (sockaddr *)&clientaddr, &len, SOCK_NONBLOCK); // 接受连接
}

string Socket::ip() const
{
    return ip_; // 返回IP地址
}

uint16_t Socket::port() const
{
    return port_; // 返回端口号
}

void Socket::setipport(const string &ip, const uint16_t &port)
{
    ip_ = ip; // 设置IP地址
    port_ = port; // 设置端口号
}