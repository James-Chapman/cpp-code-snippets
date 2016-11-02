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

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "singlelog.hpp"

void threadFunc(int c)
{
    FourtyTwo::SingletonLogger * THREAD_LOGGER = FourtyTwo::SingletonLogger::getInstance();
    std::stringstream ss;
    ss << "Thread " << c;
    std::string module = ss.str();
    THREAD_LOGGER->trace(module, "Message trace");
    THREAD_LOGGER->debug(module, "Message debug");
    THREAD_LOGGER->info(module, "Message info");
    THREAD_LOGGER->notice(module, "Message notice");
    THREAD_LOGGER->warning(module, "Message warning");
    THREAD_LOGGER->error(module, "Message error");
    THREAD_LOGGER->critical(module, "Message critical");
}

int main()
{
    FourtyTwo::SingletonLogger * MAIN_LOGGER = FourtyTwo::SingletonLogger::getInstance();
    MAIN_LOGGER->setConsoleLogLevel(FourtyTwo::LogLevel::L_NOTICE);
    MAIN_LOGGER->setLogFilePath("singlelog.log");

    // Create 10 threads that run threadFunc
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread(threadFunc, i));
    }

    // Join and exit all 10 threads
    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        it->join();
    }

    MAIN_LOGGER->info("main", "finished logging");

    return 0;
}
