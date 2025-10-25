#define THREADPOOL_MAX_NUM 12 // 定义线程池的最大线程数量

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future> // 提供 std::future 和 std::packaged_task 等功能
#include <thread>
#include <functional> // 提供 std::function 等功能

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#include <sys/syscall.h> // 定义 SYS_gettid，用于获取线程ID
#include <unistd.h>      // 定义 syscall，用于调用系统调用

// 线程池类
class ThreadPool
{
private:
    using Task = function<void()>; // 定义任务类型为无参无返回的函数

    mutex mt;                  // 互斥锁，用于保护共享资源
    condition_variable cond;   // 条件变量，用于线程同步
    string threadtype_;        // 线程池的类型名称
    atomic<bool> run = {true}; // 原子变量，表示线程池是否在运行
    atomic<int> idthr = {0};   // 原子变量，表示当前正在执行任务的线程数量

    vector<thread> thvec; // 存储线程池中的线程
    queue<Task> taskq;    // 存储任务的队列

public:
    inline ThreadPool(unsigned short size, string threadtype)
    {
        threadtype_ = threadtype; // 设置线程池类型名称
        for (; thvec.size() < THREADPOOL_MAX_NUM && size > 0; --size)
        {
            thvec.emplace_back([this]
                               {
                // 线程的执行函数
                std::cout << threadtype_ << "Thread id is created,thread D (TID): " << static_cast<pid_t>(syscall(SYS_gettid)) << std::endl;
                // 打印线程创建信息，包括线程池类型和线程ID
                while (run)
                {
                    Task task;
                    {
                        unique_lock<mutex> lok(mt); // 加锁，保护对任务队列的访问
                        cond.wait(lok, [this] {
                            return (taskq.empty() == false || !run);
                        });

                        if (taskq.empty() == true && run == false)
                            return; // 如果任务队列为空且线程池停止运行，则退出线程
                        
                        task = taskq.front();
                        taskq.pop();
                    }
                    idthr--; // 当前线程正在执行任务的数量减1
                    task();
                    idthr++; // 当前线程正在执行任务的数量加1
                } });
        }
    }

    // 线程池析构函数
    ~ThreadPool()
    {
        run = false;
        cond.notify_all();
        for (thread &thread : thvec)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    // 添加任务到线程池
    template <class F, class... Args>
    inline auto addtask(F &&f, Args &&...args) -> future<decltype(f(args...))>
    {
        if (!run)
            throw runtime_error("addtask on ThreadPool is stopped.");

        // 获取任务的返回值类型
        using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type

        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            lock_guard<mutex> lock{mt}; // 加锁，保护对任务队列的访问

            // 将任务添加到任务队列中
            taskq.emplace([task]()
                          {
                              (*task)(); // 调用包装任务
                          });
        }

        cond.notify_one();
        return task->get_future();
    }

    // 获取线程池中线程的数量
    inline int idnum()
    {
        return thvec.size();
    }

    // 获取当前正在执行任务的线程数量
    inline int idth()
    {
        return idthr;
    }
};