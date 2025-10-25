// tcpepoll.cpp
// 主程序文件，启动EchoServer。
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include "InetAddress.h"
#include "TcpServer.h"
#include "EchoServer.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("用法：./tcpepoll IP 端口\n");
        printf("示例：./tcpepoll 192.168.201.128 5085\n");
        return -1;
    }

    EchoServer echoserver(argv[1], atoi(argv[2]), 3, 0); // 创建EchoServer
    echoserver.Start(); // 启动服务器
    return 0;
}