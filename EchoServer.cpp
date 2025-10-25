// EchoServer.cpp
// EchoServer类的实现文件，实现了一个简单的回显服务器。
#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, unsigned short IOthread, unsigned short workthread) : tcpserver_(ip, port, IOthread), threadpool_(workthread, "WORK")
{
    // 设置TcpServer的回调函数
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
    // 析构函数
}

void EchoServer::Start()
{
    tcpserver_.start(); // 启动TcpServer
}

void EchoServer::HandleNewConnection(spConnection conn)
{
    cout << "新连接到来。" << endl;
}

void EchoServer::HandleClose(spConnection conn)
{
    cout << "连接已关闭。" << endl;
}

void EchoServer::HandleError(spConnection conn)
{
    cout << "连接发生错误。" << endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string message)
{
    if (threadpool_.idnum() == 0)
    {
        ONMessage(conn, message); // 在主线程中处理消息
    }
    else
    {
        threadpool_.addtask([this, conn, message]() // 将消息处理任务添加到线程池
                            {
                                ONMessage(conn, message);
                            });
    }
}

void EchoServer::ONMessage(spConnection conn, std::string message)
{
    cout << "正在进行数据库操作，请耐心等待..." << endl;
    cout << "等待发送数据：" << message << endl;
    cout << "线程ID：" << syscall(SYS_gettid) << endl;

    int len = message.size();
    string data((char *)(&len), 4);
    data.append(message);

    cout << "计算结束，正在通知事件循环，发送回应报文" << endl;
    conn->send(data.data(), data.size()); // 发送数据
}

void EchoServer::HandleSendComplete(spConnection conn)
{
    cout << "消息发送完成。" << endl;
}