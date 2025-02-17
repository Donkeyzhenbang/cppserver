#include "ThreadPool.h"

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

ThreadPool::ThreadPool(int size) : stop_(false) {
    for (int i = 0; i < size; i++) {
        //![this]：捕获当前对象的指针，使 lambda 函数能够访问 ThreadPool 的成员变量（如 tasks_mtx_ 和 cv_）。
        threads_.emplace_back(std::thread([this, i]() {
            while (true) {
                std::function<void()> task;
                {
                    // std::cout << "Thread " << i << " Starting ... \n";
                    std::unique_lock<std::mutex> lock(tasks_mtx_);
                    cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
                    if (stop_ && tasks_.empty()) return;
                    task = tasks_.front();
                    tasks_.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        stop_ = true;
    }
    cv_.notify_all();  //!唤醒所有线程，通知所有等待的线程（通过条件变量 cv_），使其能检查 stop_ 状态并退出循环。
    for (std::thread &th : threads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

// void ThreadPool::add(std::function<void()> func)
// {
//     {
//         std::unique_lock<std::mutex> lock(tasks_mtx_);
//         if(stop_)
//             throw std::runtime_error("ThreadPool already stop, can't add task any more");
//         tasks_.emplace(func);
//     }
//     cv_.notify_one(); //唤醒一个线程执行任务
// }