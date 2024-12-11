#pragma once

#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <future>

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

    // void add(std::function<void()>);
    //它接受一个可调用对象f以及可变数量的参数Args，能够返回一个std::future对象，通过这个std::future对象，
    //调用者可以在后续获取任务执行的结果（前提是可调用对象F有返回值）。std::result_of用于在编译期推导可调用对象F在传入参数Args时的返回类型。
    //!std::future用于获取异步操作。提供了一种机制，使得调用者可以在未来的某个时刻查询任务是否已经完成，
    //!并获取任务执行后的返回值（如果可调用对象 F 有返回值的话），或者处理任务执行过程中抛出的异常等情况。
    template<typename F, typename... Args>
    auto add(F&& f, Args&&... args) //!注意这里模板类是 T&&是万能引用，可以接收左值或右值
    -> std::future<typename std::result_of<F(Args...)>::type>; 
};


template<typename F, typename... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    //通过std::result_of模板类型特性来获取可调用对象F在传入参数Args后返回的结果类型，并使用using关键字定义了一个别名return_type，方便后续代码使用该类型。
    using return_type = typename std::result_of<F(Args...)>::type;//!得到返回值
    /*创建一个std::packaged_task对象，它包装了可调用对象F以及对应的参数Args，使得这个可调用对象能够以异步的方式执行，并且能够获取其执行结果。
    这里使用std::make_shared来创建一个智能指针管理这个std::packaged_task对象，确保其内存能被正确管理（特别是在多个地方可能引用它的情况下）。
    std::bind函数用于将可调用对象F和参数Args进行绑定，构造出一个新的可调用对象，std::forward用于完美转发参数，保持参数的左值或右值属性，避免不必要的拷贝或移动。*/
    auto task = std::make_shared<std::packaged_task<return_type()>>(//!智能 指针
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)//!完美转发参数
    );
    //从std::packaged_task对象中获取一个std::future对象，这个std::future对象与task关联，调用者可以通过这个std::future对象来获取task所包装的可调用对象执行后的结果
    //（例如使用res.get()来获取返回值，当然这会阻塞当前线程直到任务执行完成）。
    std::future<return_type> res = task->get_future();//!使用期约
    {//!队列锁作用域
        /*使用std::unique_lock结合互斥锁tasks_mtx_来获取对任务队列的独占访问权限。在获取锁之后，首先检查stop_变量，如果线程池已经被标记为停止（stop_为true），
        则抛出一个std::runtime_error异常，表示不应该再往已经停止的线程池中添加任务了。如果线程池未停止，通过tasks_.emplace往任务队列中添加一个新的任务，
        这个任务是一个 lambda 表达式，它捕获了task（即前面创建的std::packaged_task对象的智能指针），在 lambda 表达式的函数体中执行(*task)();
        也就是实际去调用task所包装的可调用对象来执行具体的任务。*/
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        // don't allow enqueueing after stopping the pool
        if(stop_)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks_.emplace([task](){ (*task)(); });//!任务添加到任务队列
    }
    //在成功添加任务到任务队列后，通过条件变量cv_调用notify_one函数，唤醒一个正在等待任务的线程（如果有线程在等待的话），让它去任务队列中获取新添加的任务并执行
    cv_.notify_one();//!通知一次条件变量，唤醒一个线程
    //将与任务关联的std::future对象返回给调用者，使得调用者后续可以通过这个std::future对象来获取任务的执行结果或者进行其他相关操作（比如检查任务是否执行完成等）
    return res;//!返回一个期约
}