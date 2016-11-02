/*******************************************************************************
* Copyright (c) 2014 James Chapman
*
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* 1. The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
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

/**
* Template struct containing thread function and 2 parameters
*/
template<class T1 = void, class T2 = void>
struct ThreadFunction
{
    int params = 2;
    std::function<void(T1 &, T2 &)> function_name;
    T1 function_param1;
    T2 function_param2;
    void executeFunction()
    {
        function_name(function_param1, function_param2);
    }
};


/**
* Template struct containing thread function and 1 parameter
*/
template<class T1>
struct ThreadFunction<T1, void>
{
    int params = 1;
    std::function<void(T1 &)> function_name;
    T1 function_param1;
    void executeFunction()
    {
        function_name(function_param1);
    }
};


/**
* Template struct containing thread function and no parameters
*/
template<>
struct ThreadFunction<void, void>
{
    int params = 0;
    std::function<void()> function_name;
    void executeFunction()
    {
        function_name();
    }
};


typedef ThreadTesting::ThreadFunction<> ThreadFunction_noParam;
typedef ThreadTesting::ThreadFunction<int> ThreadFunction_p1Int;
typedef ThreadTesting::ThreadFunction<int, int> ThreadFunction_p1Int_p2Int;
typedef ThreadTesting::ThreadFunction<std::string> ThreadFunction_p1String;
typedef ThreadTesting::ThreadFunction<std::string, std::string> ThreadFunction_p1String_p2String;


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
