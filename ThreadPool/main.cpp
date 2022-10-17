/*******************************************************************************
 * Copyright (c) 2014 James Chapman
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

#include "commonthreadpool.hpp"

#include <functional>
#include <iostream>
#include <random>

namespace
{

int threadFunctionOneParamReturnsInt(int i)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " running with parameter: " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 1;
}

void threadFunctionOneParam(int i)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " running with parameter: " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

struct Data
{
    int i{99};
    char c{'A'};
    void print()
    {
        std::cout << "Data.print() i=" << i << ", c=" << c << "\n";
    }
};

void threadFunctionTwoParams(Data d, double n)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " Data object and double " << n << "\n";
    d.print();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

} // namespace

int main()
{
    // random number generator
    std::random_device rd;
    std::mt19937 urng(rd());

    // Create a threadpool with 5 threads
    ::Uplinkzero::Common::ThreadPool threadPool(5);

    // enque tasks into the threadpool
    for (int i = 0; i < 10; i++)
    {
        auto n = urng();
        threadPool.enqueue(threadFunctionOneParamReturnsInt, n);
        threadPool.enqueue(threadFunctionOneParam, n);
        Data d;
        threadPool.enqueue(threadFunctionTwoParams, d, n);
        threadPool.enqueue([]() {
            std::cout << "Thread ID: " << std::this_thread::get_id() << " lambda\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Main thread finished!" << std::endl;

    return 0;
}
