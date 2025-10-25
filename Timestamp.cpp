// Timestamp.cpp
// Timestamp类的实现文件，封装了时间戳操作。
#include <time.h>
#include "Timestamp.h"

Timestamp::Timestamp()
{
    secsinceepoch_ = time(0); // 获取当前时间
}

Timestamp::Timestamp(int64_t secsinceepoch) : secsinceepoch_(secsinceepoch)
{
    // 构造函数
}

Timestamp Timestamp::now()
{
    return Timestamp(); // 返回当前时间
}

time_t Timestamp::toint() const
{
    return secsinceepoch_; // 返回时间戳
}

std::string Timestamp::tostring() const
{
    char buf[32] = {0};
    tm *tm_time = localtime(&secsinceepoch_);
    snprintf(buf, 20, "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
    return buf; // 返回时间字符串
}