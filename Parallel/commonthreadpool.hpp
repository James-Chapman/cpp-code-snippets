/*******************************************************************************
* Author      : James Chapman
* License     : BSD
* Date        : 17 November 2014
* Description : Common class for Managing threads.
********************************************************************************/

#ifndef COMMONTHREADPOOL_HPP
#define COMMONTHREADPOOL_HPP

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace ThreadTesting
{

template<class T>
class ThreadPool;

/**
* Worker
*/
template<class T>
class Worker {
public:
    Worker(ThreadPool<T> &s) : m_threadPool(s) { }
    void operator()()
    {
        T task;
        while (1)
        {
            // Get the work to be done
            {
                std::unique_lock<std::mutex> lock(m_threadPool.queue_mutex);

                while (!m_threadPool.stop && m_threadPool.taskQueue.empty())
                {
                    m_threadPool.condition.wait(lock);
                }

                if (m_threadPool.stop)
                {
                    return;
                }

                task = m_threadPool.taskQueue.front();
                m_threadPool.taskQueue.pop();
            }

            // Do the work
            task.function_name(task.function_param);
        }
    }

private:
    ThreadPool<T> &m_threadPool;
};


/**
* Threadpool
*/
template<class T>
class ThreadPool {
public:
    ThreadPool(size_t _threads) : stop(false)
    {
        for (size_t i = 0; i < _threads; ++i)
        {
            workers.push_back(std::thread(Worker<T>(*this)));
        }
    }

    ~ThreadPool()
    {
        stop = true;
        condition.notify_all();
        for (size_t i = 0; i < workers.size(); ++i)
        {
            workers[i].join();
        }
    }

    void enqueue(T _taskItem)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            taskQueue.push(_taskItem);
        }
        condition.notify_one();
    }

private:
    template<class T> friend class Worker;
    std::vector<std::thread> workers;
    std::queue<T> taskQueue;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

} // end namespace Testing



#endif // COMMONTHREADPOOL_HPP
