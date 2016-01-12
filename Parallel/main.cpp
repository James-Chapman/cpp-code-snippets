/*******************************************************************************
* Author      : James Chapman
* License     : BSD
* Date        : 17 November 2014
* Description : Common class for Managing threads.
********************************************************************************/

#include <iostream>
#include <random>

#include "CommonThreadPool.hpp"

namespace ThreadTesting
{

    /**
    * Template struct which contains the thread function to call along with parameter.
    * Param could be anything. Currently limited to a single parameter.
    */
    template<class T>
    struct ThreadTask
    {
        std::function<void(T &)> function_name;
        T function_param;
    };

}

void functionForThreadToRun(int _i)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " running with parameter: " << _i << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main()
{
    typedef ::ThreadTesting::ThreadTask<int> ThreadData;

    // random number generator
    std::random_device rd;
    std::mt19937 urng(rd());

    // Create a threadpool with 5 threads
    ::ThreadTesting::ThreadPool<ThreadData> threadPool(5);
        
    // Pointer to the function we want the threads in the pool to execute
    auto pFunctionForThreadToRun = functionForThreadToRun;

    // enque tasks into the threadpool
    for (int i = 0; i < 1000; i++)
    {
        ThreadData td;
        td.function_name = pFunctionForThreadToRun;
        td.function_param = urng();
        threadPool.enqueue(td);
    }

    // sleep for 10 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    std::cout << "Main thread finished!" << std::endl;

    return 0;
}
