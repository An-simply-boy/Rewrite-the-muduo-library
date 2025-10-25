// Buffer.cpp
// Buffer类的实现文件，用于管理数据缓冲区。
#include "Buffer.h"

Buffer::Buffer()
{
    // 构造函数
}

Buffer::~Buffer()
{
    // 析构函数
}

void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size); // 将数据追加到缓冲区
}

size_t Buffer::size()
{
    return buf_.size(); // 返回缓冲区大小
}

const char *Buffer::data()
{
    return buf_.data(); // 返回缓冲区首地址
}

void Buffer::clear()
{
    buf_.clear(); // 清空缓冲区
}

void Buffer::erase(size_t pos, size_t nn)
{
    buf_.erase(pos, nn); // 删除缓冲区中的数据
}