// Buffer.h
// Buffer类的头文件，用于管理数据缓冲区。
#pragma once
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf_; // 缓冲区

public:
    Buffer(); // 构造函数
    ~Buffer(); // 析构函数

    void append(const char *data, size_t size); // 追加数据到缓冲区
    size_t size(); // 返回缓冲区大小
    const char *data(); // 返回缓冲区首地址
    void clear(); // 清空缓冲区
    void erase(size_t pos, size_t nn); // 删除缓冲区中的数据
};