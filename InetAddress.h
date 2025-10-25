// InetAddress.h
// InetAddress类的头文件，封装了网络地址。
#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in addr_; // 网络地址结构体

public:
    InetAddress(const std::string &ip, uint16_t port); // 构造函数，从IP和端口初始化
    InetAddress(const sockaddr_in addr); // 构造函数，从sockaddr_in结构体初始化
    ~InetAddress(); // 析构函数

    const char *ip() const; // 返回IP地址字符串
    uint16_t port() const; // 返回端口号
    const sockaddr *addr() const; // 返回sockaddr结构体
};