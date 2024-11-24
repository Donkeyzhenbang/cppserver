#pragma once

#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>

class ThreadPool
{
private:
    std::vector<std::thread> threads_;              //线程存在vector
    std::queue<std::function<void()>> tasks_;       //任务队列
    std::mutex tasks_mtx_;                          //互斥锁
    std::condition_variable cv_;                    //条件变量
    bool stop_;
public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    void add(std::function<void()>);
};
