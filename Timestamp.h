// Timestamp.h
// Timestamp类的头文件，封装了时间戳操作。
#pragma once

#include <iostream>
#include <string>

class Timestamp
{
private:
    time_t secsinceepoch_; // 时间戳

public:
    Timestamp(); // 构造函数
    Timestamp(int64_t secsinceepoch); // 构造函数

    static Timestamp now(); // 返回当前时间

    time_t toint() const; // 返回时间戳
    std::string tostring() const; // 返回时间字符串
};