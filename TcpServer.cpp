// TcpServer.cpp
// TcpServer类的实现文件，封装了TCP服务器的逻辑。
#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port, unsigned short threadnum) : pool_(threadnum, "IO")
{
    acceptor_ = new Acceptor(&loop_, ip, port); // 创建Acceptor
    acceptor_->setnewconnectioncb(bind(&TcpServer::newconnection, this, placeholders::_1)); // 设置新连接回调
    loop_.settimeout(true, 5); // 设置超时
    for (int i = 0; i < pool_.idnum(); i++)
    {
        subloop_.push_back(std::make_unique<EventLoop>()); // 创建子事件循环
        auto task = std::make_shared<std::packaged_task<void()>>(std::bind(&EventLoop::run, subloop_[i].get())); // 创建任务
        pool_.addtask([task]() { (*task)(); }); // 将任务加入线程池
        subloop_[i]->settimeout(false, 10); // 设置超时
    }
}

TcpServer::~TcpServer()
{
    // 析构函数
}

void TcpServer::start()
{
    loop_.run(); // 运行事件循环
}

void TcpServer::newconnection(Socket *clientsock)
{
    spConnection conn(new Connection(subloop_[clientsock->fd() % pool_.idnum()].get(), clientsock)); // 创建连接
    printf("新连接(fd=%d,ip=%s,port=%d)已建立。\n", clientsock->fd(), conn->ip().c_str(), conn->port());
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, placeholders::_1)); // 设置关闭回调
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, placeholders::_1)); // 设置错误回调
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, placeholders::_1, placeholders::_2)); // 设置消息回调
    conns[conn->fd()] = conn; // 添加连接
    newconnectioncb_(conn); // 调用新连接回调
    subloop_[clientsock->fd() % pool_.idnum()]->newconnewcton(conn->fd(), conn); // 添加连接到子事件循环
    subloop_[clientsock->fd() % pool_.idnum()]->settimeout(std::bind(&TcpServer::handeltimeout, this, placeholders::_1)); // 设置超时
}

void TcpServer::closeconnection(spConnection conn)
{
    closeconnectioncb_(conn); // 调用关闭回调
    conns.erase(conn->fd()); // 删除连接
}

void TcpServer::errorconnection(spConnection conn)
{
    errorconnectioncb_(conn); // 调用错误回调
    conns.erase(conn->fd()); // 删除连接
}

void TcpServer::onmessage(spConnection conn, string message)
{
    onmessagecb_(conn, message); // 调用消息回调
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_ = fn; // 设置新连接回调
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_ = fn; // 设置关闭回调
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_ = fn; // 设置错误回调
}

void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> fn)
{
    onmessagecb_ = fn; // 设置消息回调
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_ = fn; // 设置发送完成回调
}

void TcpServer::settimeoutcb(std::function<void(EventLoop *)> fn)
{
    timeoutcb_ = fn; // 设置超时回调
}

void TcpServer::handeltimeout(spConnection conn)
{
    lock_guard<mutex> lock{mt}; // 加锁
    conns.erase(conn->fd()); // 删除连接
}