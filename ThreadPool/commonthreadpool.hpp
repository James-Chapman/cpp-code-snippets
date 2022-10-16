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

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace Uplinkzero
{

namespace Common
{

    /**
     * Edit this struct to contain required params
     */
    struct FunctionParams
    {
        unsigned long i;
    };

    /**
     * Class containing thread function and parameters
     */
    class ThreadData
    {
      public:
        ThreadData() : m_functionParams(){};
        virtual ~ThreadData(){};

        void setThreadFunction(const std::function<void(FunctionParams&)>& f)
        {
            m_functionName = f;
        }

        void setThreadFunctionParams(FunctionParams p)
        {
            m_functionParams = p;
        }

        void executeFunction()
        {
            m_functionName(m_functionParams);
        }

      private:
        std::function<void(FunctionParams&)> m_functionName;
        FunctionParams m_functionParams;
    };

    template <typename T> class ThreadPool;

    /**
     * Worker
     */
    template <typename T> class Worker
    {
      public:
        explicit Worker(ThreadPool<T>& s) : m_threadPool(s)
        {
        }
        void operator()()
        {
            T task;
            while (1)
            {
                // Get the work to be done
                {
                    std::unique_lock<std::mutex> lock(m_threadPool.m_queueMutex);

                    while (!m_threadPool.m_stop && m_threadPool.m_taskQueue.empty())
                    {
                        m_threadPool.m_condition.wait(lock);
                    }

                    if (m_threadPool.m_stop)
                    {
                        return;
                    }

                    task = m_threadPool.m_taskQueue.front();
                    m_threadPool.m_taskQueue.pop();
                }

                // Do the work
                task.executeFunction();
            }
        }

      private:
        ThreadPool<T>& m_threadPool;
    };

    /**
     * Threadpool
     */
    template <typename T> class ThreadPool
    {
      public:
        explicit ThreadPool(size_t _threads) : m_stop(false)
        {
            for (size_t i = 0; i < _threads; ++i)
            {
                m_workers.push_back(std::thread(Worker<T>(*this)));
            }
        }

        ~ThreadPool()
        {
            m_stop = true;
            m_condition.notify_all();
            for (size_t i = 0; i < m_workers.size(); ++i)
            {
                m_workers[i].join();
            }
        }

        void enqueue(T _taskItem)
        {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_taskQueue.push(_taskItem);
            }
            m_condition.notify_one();
        }

      private:
        template <class T> friend class Worker;
        std::vector<std::thread> m_workers;
        std::queue<T> m_taskQueue;
        std::mutex m_queueMutex;
        std::condition_variable m_condition;
        bool m_stop;
    };

} // end namespace Common

} // end namespace Uplinkzero
