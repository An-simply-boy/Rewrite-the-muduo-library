// Socket.h
// Socket类的头文件，封装了套接字操作。
#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

using namespace std;

int createnonblocking(); // 创建非阻塞套接字

class Socket
{
private:
    const int fd_; // 文件描述符
    string ip_; // IP地址
    uint16_t port_; // 端口号

public:
    Socket(int fd); // 构造函数
    ~Socket(); // 析构函数

    int fd() const; // 返回文件描述符
    void settcpnodelay(bool on); // 设置TCP_NODELAY选项
    void setreuseaddr(bool on); // 设置SO_REUSEADDR选项
    void setreuseport(bool on); // 设置SO_REUSEPORT选项
    void setkeepalive(bool on); // 设置SO_KEEPALIVE选项
    void bind(const InetAddress &servaddr); // 绑定地址
    void listen(int nn = 128); // 开始监听
    int accept(InetAddress &clientaddr); // 接受连接

    string ip() const; // 返回IP地址
    uint16_t port() const; // 返回端口号
    void setipport(const string &ip, const uint16_t &port); // 设置IP和端口
};