// Connection.cpp
// Connection类的实现文件，负责管理单个客户端连接。
#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock) : loop_(loop), clientsock_(clientsock), clientchannel_(new Channel(loop_, clientsock_->fd()))
{
    clientchannel_->useet(); // 启用边缘触发
    clientchannel_->enablereading(); // 启用读事件
    clientchannel_->setreadcallback(bind(&Connection::onmessage, this)); // 设置读事件回调
    clientchannel_->setwritecallback(bind(&Connection::writecallback, this)); // 设置写事件回调
    clientchannel_->setclosecallback(bind(&Connection::closecallback, this)); // 设置关闭事件回调
    clientchannel_->seterrorcallback(bind(&Connection::errorcallback, this)); // 设置错误事件回调
}

Connection::~Connection()
{
    cout << "调用了析构函数" << endl;
}

string Connection::ip() const
{
    return clientsock_->ip(); // 返回客户端IP
}

uint16_t Connection::port() const
{
    return clientsock_->port(); // 返回客户端端口
}

int Connection::fd() const
{
    return clientsock_->fd(); // 返回文件描述符
}

void Connection::closecallback()
{
    printf("连接(eventfd=%d)已断开。\n", this->fd());
    close(this->fd()); // 关闭文件描述符
    closecallback_(shared_from_this()); // 调用关闭事件回调
}

void Connection::errorcallback()
{
    errorcallback_(shared_from_this()); // 调用错误事件回调
    printf("连接(eventfd=%d)已断开。\n", this->fd());
    close(this->fd()); // 关闭文件描述符
    delete this; // 删除当前对象
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    closecallback_ = fn; // 设置关闭事件回调
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    errorcallback_ = fn; // 设置错误事件回调
}

void Connection::onmessage()
{
    char buffer[1024];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(this->fd(), buffer, sizeof(buffer));
        if (nread > 0)
        {
            inputbuffer_.append(buffer, nread); // 将读取的数据追加到输入缓冲区
        }
        else if (nread == -1 && errno == EINTR)
        {
            continue; // 读取被信号中断，继续读取
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            while (true)
            {
                if (inputbuffer_.size() < 4) break;
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if (inputbuffer_.size() < len + 4) break;
                string str(inputbuffer_.data() + 4, len);
                ts = Timestamp::now();
                printf("收到数据(eventfd=%d): %s", fd(), str.c_str());
                onmessagecallback_(shared_from_this(), str); // 调用消息处理回调
                inputbuffer_.erase(0, len + 4); // 从输入缓冲区中删除已处理的数据
            }
            break;
        }
        else if (nread == 0)
        {
            closecallback(); // 客户端断开连接，调用关闭事件回调
            break;
        }
    }
}

void Connection::writecallback()
{
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0); // 尝试发送数据
    if (writen > 0)
        outputbuffer_.erase(0, writen); // 从输出缓冲区中删除已发送的数据
    if (outputbuffer_.size() == 0)
        clientchannel_->disablewriting(); // 如果输出缓冲区为空，禁用写事件
}

void Connection::setonmessagecallback(std::function<void(spConnection, std::string)> fn)
{
    onmessagecallback_ = fn; // 设置消息处理回调
}

void Connection::send(const char *data, size_t size)
{
    if (loop_->isinloopthread())
    {
        printf("send()在事件循环线程中操作缓冲区\n");
        outputbuffer_.append(data, size); // 将数据追加到输出缓冲区
        clientchannel_->enablewriting(); // 启用写事件
    }
    else
    {
        printf("send()不在事件循环线程中操作缓冲区\n");
        string str(&data[4], size - 4);
        cout << "服务端回应：" << str << endl;
        char *message = new char[size];
        memcpy(message, data, size);
        loop_->queueinloopthread(bind(&Connection::sendinloop, this, message, size)); // 将发送操作排队到事件循环线程
    }
}

void Connection::sendinloop(const char *data, size_t size)
{
    outputbuffer_.append(data, size); // 将数据追加到输出缓冲区
    clientchannel_->enablewriting(); // 启用写事件
    string str(&data[4], size - 4);
    cout << "服务端回应：" << str << endl;
    delete data; // 删除动态分配的数据
}

int Connection::laststamp()
{
    return ts.toint(); // 返回时间戳
}