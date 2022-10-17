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
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace Uplinkzero
{

namespace Common
{
    class ThreadPool;

    /**
     * ThreadWorker - runs in the thread and executes the requested function
     */
    class __ThreadWorker
    {
    public:
        explicit __ThreadWorker(ThreadPool& s);
        void operator()();

    private:
        ThreadPool& m_threadPool;
    };

    /**
     * Threadpool - a pool of Workers, each in it's own thread
     */
    class ThreadPool
    {
    public:
        explicit ThreadPool(size_t threads) : m_stop(false)
        {
            for (size_t i = 0; i < threads; ++i)
            {
                m_workers.push_back(std::thread(__ThreadWorker(*this)));
            }
        }

        virtual ~ThreadPool()
        {
            m_stop = true;
            m_condition.notify_all();
            for (size_t i = 0; i < m_workers.size(); ++i)
            {
                m_workers[i].join();
            }
        }

        ThreadPool(ThreadPool const&) = delete;
        ThreadPool& operator=(const ThreadPool& rhs) = delete;

        template <typename F, typename... TArgs>
        void enqueue(F func, TArgs&&... args)
        {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_taskQueue.push(std::bind(func, std::forward<TArgs>(args)...));
            }
            m_condition.notify_one();
        }

    protected:
    private:
        std::mutex m_queueMutex;
        friend class __ThreadWorker;
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_taskQueue;
        std::condition_variable m_condition;
        bool m_stop;
    };

    /**
     * ThreadWorker implementation
     */

    __ThreadWorker::__ThreadWorker(ThreadPool& s) : m_threadPool(s)
    {
    }

    void __ThreadWorker::operator()()
    {
        while (1)
        {
            std::function<void()> task;
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
            task();
        }
    }

} // namespace Common

} // namespace Uplinkzero
