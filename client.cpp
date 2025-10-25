// client.cpp
// 客户端实现文件，用于连接服务器并发送/接收数据。
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

// 客户端套接字类
class ClientSocket
{
private:
    int sockfd; // 客户端套接字文件描述符

public:
    // 连接到服务器
    bool connects(const string &host, const unsigned short port)
    {
        // 创建套接字
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            perror("socket");
            return false;
        }

        // 获取服务端地址
        struct hostent *h;
        if ((h = gethostbyname(host.c_str())) == 0)
        {
            cout << "gethostbyname failed.\n";
            return false;
        }

        // 设置服务端地址
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
        servaddr.sin_port = htons(port);

        // 连接服务端
        if (::connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        {
            perror("connect");
            ::close(sockfd);
            return false;
        }
        cout << "连接成功" << endl;
        return true;
    }

    // 发送数据
    bool send(const string &data)
    {
        int iret;
        if ((iret = ::send(sockfd, data.data(), data.size(), 0)) <= 0)
        {
            perror("send");
            return false;
        }
        return true;
    }

    // 发送带长度的数据
    bool send(int len, const string &data)
    {
        int iret;
        char data_[1024];
        memset(data_, 0, sizeof(data_));
        memcpy(data_, &len, 4);
        memcpy(data_ + 4, data.data(), len);
        if ((iret = ::send(sockfd, data_, len + 4, 0)) <= 0)
        {
            perror("send");
            return false;
        }
        return true;
    }

    // 接收数据
    ssize_t recv(string &data, const size_t maxlen)
    {
        data.clear();
        data.resize(maxlen);
        ssize_t iret;
        iret = ::recv(sockfd, &data[0], maxlen, 0);
        return iret;
    }

    // 关闭套接字
    void close()
    {
        sockfd = -1;
        ::close(sockfd);
    }

    // 析构函数，确保套接字关闭
    ~ClientSocket()
    {
        close();
    }

    // 获取套接字文件描述符
    int getfd()
    {
        return sockfd;
    }
};

// 分割接收到的数据
void divistr(string &buffer)
{
    while (1)
    {
        if (buffer.size() < 4)
            break;
        int len;
        memcpy(&len, buffer.data(), 4);
        if (buffer.size() < len + 4)
            break;
        string str(buffer.data() + 4, len);
        cout << "收到回应：" << str << endl;
        buffer.erase(0, len + 4);
    }
}

// 主函数
int main(int argc, char *argv[])
{
    // 检查参数
    if (argc != 3)
    {
        cout << "用法：./client 服务端IP 服务端端口\n示例：./client 192.168.201.128 5085\n";
        return -1;
    }

    // 创建客户端套接字
    ClientSocket cs;
    if (!cs.connects(argv[1], (unsigned short)atoi(argv[2])))
    {
        return 0;
    }

    // 发送数据
    char buff[1024];
    for (int i = 0; i <= 80; i++)
    {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "测试数据%d\n", i);
        int len = strlen(buff);
        if (!cs.send(len, buff))
        {
            cout << "发送失败" << endl;
            return 0;
        }
    }

    // 接收数据
    for (int i = 1; i <= 100; i++)
    {
        string buffer;
        string str;
        ssize_t size;
        size = cs.recv(str, 1024);
        buffer.append(str.data(), size);
        divistr(buffer);
    }

    // 关闭连接
    cs.close();
    return 0;
}