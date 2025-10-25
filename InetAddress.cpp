// InetAddress.cpp
// InetAddress类的实现文件，封装了网络地址。
#include "InetAddress.h"

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET; // IPv4协议
    addr_.sin_addr.s_addr = inet_addr(ip.c_str()); // 设置IP地址
    addr_.sin_port = htons(port); // 设置端口
}

InetAddress::InetAddress(const sockaddr_in addr) : addr_(addr)
{
    // 从sockaddr_in结构体初始化
}

InetAddress::~InetAddress()
{
    // 析构函数
}

const char *InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr); // 返回IP地址字符串
}

uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port); // 返回端口号
}

const sockaddr *InetAddress::addr() const
{
    return (sockaddr *)&addr_; // 返回sockaddr结构体
}