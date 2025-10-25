// Acceptor.cpp
// Acceptor类的实现文件，负责监听新连接并通知TcpServer。
#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port) : loop_(loop), servsock_(createnonblocking()), acceptchannel_(loop_, servsock_.fd())
{
    InetAddress servaddr(ip, port); // 创建服务端地址
    servsock_.setreuseaddr(true);  // 设置SO_REUSEADDR选项
    servsock_.settcpnodelay(true); // 设置TCP_NODELAY选项
    servsock_.setreuseport(true);  // 设置SO_REUSEPORT选项
    servsock_.setkeepalive(true);  // 设置SO_KEEPALIVE选项
    servsock_.bind(servaddr);      // 绑定地址
    servsock_.listen();            // 开始监听

    acceptchannel_.setreadcallback(bind(&Acceptor::newconnection, this)); // 设置读事件回调
    acceptchannel_.enablereading();                                      // 启用读事件监听
}

Acceptor::~Acceptor()
{
    // 析构函数
}

void Acceptor::setnewconnectioncb(std::function<void(Socket *)> fn)
{
    newconnectioncb_ = fn; // 设置新连接回调函数
}

void Acceptor::newconnection()
{
    sockaddr_in peeraddr; // 客户端地址
    InetAddress clientaddr(peeraddr); // 创建客户端地址对象
    Socket *clientsock = new Socket(servsock_.accept(clientaddr)); // 接受新连接
    clientsock->setipport(clientaddr.ip(), clientaddr.port());     // 设置客户端地址和端口
    newconnectioncb_(clientsock);                                  // 调用新连接回调
}